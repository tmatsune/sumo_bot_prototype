#include "i2c.h"

void i2c_init(void){
    // Set SCL frequency

}


bool i2c_write(uint8_t data){
    TWDR = data; // COPY DATA INTO TWI DATA REGISTER
    // TWINT CLEARS BIT, NEED TO WRITE NEW DATA 
    // TWEN ENABLES TWI COMMUNICATION
    TWCR = (1 << TWINT) | (1 << TWEN); 
    int count = 0;
    while(count < 20 & ( TWCR & (1 << TWINT)) == 0 ){ // WAIT TILL DATA HAS BEEN SENT, WILL CONTUNUE UNTIL TWINT == 1
        count++;
    }; 
};

bool i2c_start(void){
    // TWINT CLEARS BIT NEED TO WRITE NEW DATA
    // TWEN ENABLES TWI COMMUNICATION
    // TWSTA start condition initiates communication on the TWI bus. It signifies the beginning of a new data transfer sequence.
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    int count = 0;
    while(count < 20 & (TWCR & (1 << TWINT)) == 0) { // CHECK WHETHER START CONDITION HAS BEEN GENERATED
        count++;
    }
    return count < 20;
}

bool i2c_stopp(void){
    // TWSTO generates a stop condition, bit is cleared by hardware when stop condition is transmitted
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    int count = 0;
    while (count < 20 & TWCR & (1 << TWSTO)){
        count++;
    }
    return count < 20;
}

#include "line_sensors.h"
#define LINE_THRESHOLD (700u)
 
void line_sensors_init(void){
    adc_init();
}
   
Line_Pos get_lines(){
    adc_input_values adc_vals;
    get_adc_input_values(adc_vals); // adc_vals[2] test
    
    const bool front_left = adc_vals[0] < LINE_THRESHOLD;
    const bool front_right = adc_vals[1] < LINE_THRESHOLD;
    const bool back_left = adc_vals[2] < LINE_THRESHOLD;
    const bool back_right = adc_vals[3] < LINE_THRESHOLD;

    if(front_left){
        if(front_right) return LINE_FRONT;
        else if(back_left) return LINE_LEFT;
        else return LINE_FRONT_LEFT;
    } else if(front_right){
        if(back_right)return LINE_RIGHT;
        return LINE_FRONT_RIGHT;
    } else if(back_left){
        if(back_left) return LINE_BACK;
        return LINE_BACK_LEFT;
    }else if(back_right){
        return LINE_BACK_RIGHT;
    }
    return LINE_NONE;
}

const char *line_str(Line_Pos line){
    switch(line){
        case LINE_NONE:
            return "LINE_NONE";
        case LINE_BACK:
            return "LINE_BACK";
        case LINE_BACK_LEFT:
            return "LINE_BACK_LEFT";
        case LINE_BACK_RIGHT:
            return "LINE_BACK_RIGHT";
        case LINE_FRONT:
            return "LINE_FRONT";
        case LINE_FRONT_LEFT:
            return "LINE_FRONT_LEFT";
        case LINE_FRONT_RIGHT:
            return "LINE_FRNO_RIGHT";
        case LINE_LEFT:
            return "LINE_LEFT";
        case LINE_RIGHT:
            return "LINE_RIGHT";
        case LINE_DIAGONAL_LEFT:
            return "LINE_DIAGONAL_LEFT";
        case LINE_DIAGONAL_RIGHT:
            return "LINE_DIAGONAL_RIGHT";
        default: 
            return "LINE_NONE";
    }
    return "LINE_NONE";
}

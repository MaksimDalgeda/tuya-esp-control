#include <signal.h>

#include "system_info_service.h" 
#include "system_info_convertor.h"
#include "ubus.h"

Error system_info_service(system_info_t * info)
{
    Error_Code ret;
    Error err;

        ret = get_system_info(info);
        err = parser_error_code(ret);

    return err;
}

Error initialize_ubus_connection(){

    Error_Code ret = OK;
    Error err;

    ret = initialize_ubus(); 
    if(ret != OK){
        err = parser_error_code(ret);
        return err;
    }

    ret = ubus_get_object_ids();
    if(ret != OK){
        err = parser_error_code(ret);
        return err;
    }

    return OK;
}

void initialize_ubus_disconnect(){

    disconnect_ubus(); 

}
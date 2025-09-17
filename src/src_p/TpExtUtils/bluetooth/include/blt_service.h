#ifndef _BLUET_SERVICE_H_
#define _BLUET_SERVICE_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>
#include "service.h"


#define SERVICE_SCANNER_TYPE (service_scanner_get_type())


typedef struct _ServiceScannerPrivate ServiceScannerPrivate;

struct _ServiceScanner {
    GObject parent_instance;
    
    /*< private >*/
    ServiceScannerPrivate *priv;
};


G_DECLARE_FINAL_TYPE(ServiceScanner, service_scanner, SERVICE, SCANNER, GObject)

ServiceScanner *service_scanner_new(void);
void service_scanner_start(ServiceScanner *scanner);
void service_scanner_stop(ServiceScanner *scanner);
GList *service_scanner_get_services(ServiceScanner *scanner);


int blt_service_test() ;



#ifdef	__cplusplus
}
#endif

#endif
//
//  main.m
//  AWeibo
//
//  Created by Loki on 12-5-14.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "cpe/pal/pal_dlfcn.h"
#import "gd/app/app_context.h"
#import "AppDelegate.h"

void ios_log(struct error_info * info, void * context, const char * fmt, va_list args) {
	char buf[1024];
    size_t s;
    s = 0;

    if (info->m_file) s += snprintf(&buf[0], sizeof(buf), "%s:%d: ", info->m_file, info->m_line > 0 ? info->m_line : 0);
    else if (info->m_line >= 0) s += snprintf(buf + s, sizeof(buf) - s, "%d: ", info->m_line);

	vsnprintf(buf + s, sizeof(buf) - s, fmt, args);

	NSLog(@"%s", buf);
}

int ios_app_main(gd_app_context_t ctx, void * user_ctx) {
    @autoreleasepool {
        return UIApplicationMain(
            gd_app_argc(ctx), gd_app_argv(ctx),
            nil, NSStringFromClass([AppDelegate class]));
    }
}

int main(int argc, char *argv[])
{
    gd_app_context_t app_ctx = gd_app_context_create(NULL, 0, argc, argv);
    if (app_ctx == NULL) {
        printf("create app ctx fail!");
        return -1;
    }
    gd_set_default_library(dlopen(NULL, RTLD_NOW));
    gd_app_set_main(app_ctx, ios_app_main, NULL, NULL);

    CPE_DEF_ERROR_MONITOR(em, cpe_error_log_to_consol_and_flush, NULL);
    gd_app_set_em(app_ctx, &em);

    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
 
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *bundlePath = [bundle bundlePath];
  
    gd_app_set_root(app_ctx, [bundlePath cStringUsingEncoding:NSASCIIStringEncoding]);
    
    int ret = gd_app_run(app_ctx);

    [bundle release];
    [pool release];
    
    gd_app_context_free(app_ctx);
 
    return ret;  
}

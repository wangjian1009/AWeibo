product:=AWeibo
$(product).type:=progn install
$(product).depends:=libmmseg \
                    cpepp_cfg \
                    gd_app gd_evt gd_net gdpp_nm cpepp_dp gdpp_app gdpp_timer
$(product).libraries:=
$(product).c.sources:= $(wildcard $(product-base)/ios/*.m)
$(product).c.frameworks:=CoreGraphics UIKit Foundation
$(product).c.flags.cpp:=-I$(CPDE_ROOT)

$(product).c.flags.ld:=
$(product).c.export-symbols:= \
                       $(foreach m,\
                            app_setup \
                            gd_evt_mgr \
                            , $m_app_init $m_app_fini) \
                        app_net_env_global_init app_net_env_global_fini

$(product).output:=AWeibo.app
$(product).source-r-dirs:=AWeibo/Resources/etc
$(product).target-r-dirs:=AWeibo.app/etc
$(product).install:= $(call def-cvt-file,AWeibo/ios/AWeibo-Info.plist, AWeibo.app/Info.plist, AWeibo/product.def) \
                     $(call def-copy-dir-r,Resources/etc, Tank.app/etc) \
                     $(call def-copy-dir,Resources, AWeibo.app, png PNG caf txt mp3)

$(eval $(call product-def,$(product),product))

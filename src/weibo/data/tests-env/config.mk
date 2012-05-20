product:=testenv.weibo_data
$(product).type:=lib
$(product).buildfor:=dev
$(product).depends:=testenv.gd_app \
                    weibo_data
$(product).c.sources:=$(wildcard $(product-base)/*.cpp)

$(eval $(call product-def,$(product)))

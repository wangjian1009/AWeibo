product:=weibo_sina_data
$(product).type:=lib
$(product).depends:=weibo_data
$(product).c.flags.ld:=
$(product).c.sources:=$(wildcard $(product-base)/*.c)

#$(eval $(call product-def,$(product)))

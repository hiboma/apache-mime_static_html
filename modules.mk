mod_mime_static_html.la: mod_mime_static_html.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_mime_static_html.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_mime_static_html.la

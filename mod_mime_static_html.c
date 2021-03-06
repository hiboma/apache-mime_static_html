/* 
**  mod_mime_static_html.c -- Apache sample mime_static_html module
**  [Autogenerated via ``apxs -n mime_static_html -g'']
**
**  To play with this sample module first compile it into a
**  DSO file and install it into Apache's modules directory 
**  by running:
**
**    $ apxs -c -i mod_mime_static_html.c
**
**  Then activate it in Apache's httpd.conf file for instance
**  for the URL /mime_static_html in as follows:
**
**    #   httpd.conf
**    LoadModule mime_static_html_module modules/mod_mime_static_html.so
**
**  Then after restarting Apache via
**
**    $ apachectl restart
**
*/

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include "http_log.h"

static apr_table_t *table = NULL;

static apr_status_t mime_static_html_send_error_document(request_rec * r,
                                                         const char *path)
{
    apr_status_t status;
    apr_finfo_t finfo;
    apr_file_t *fd = NULL;
    conn_rec *c = r->connection;
    apr_bucket_brigade *bb;
    apr_bucket *e;
    char buffer[4096];

    /* ファイルがあるかどうか */
    status = apr_stat(&finfo, path, APR_FINFO_TYPE, r->pool);
    if (APR_SUCCESS != status) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, status, r,
                      "AddMIMEStatic '%s'", path);
        return APR_ENOSTAT;
    }

    /* 通常ファイル以外 .. ディレクトリなんぞを指定した場合 */
    if (APR_REG != finfo.filetype) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                      "mod_mime_static_html.c Attempt to serve non regular file: %s",
                      path);
        return APR_EGENERAL;
    }

    /* ファイルオープン */
    status = apr_file_open(&fd, path, APR_READ, 0, r->pool);
    if (APR_SUCCESS != status) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, status, r,
                      "mod_mime_static_html.c file permissions deny server access: %s",
                      r->filename);
        return APR_EGENERAL;
    }

    r->content_type = "text/html";
    while (apr_file_gets(buffer, sizeof(buffer), fd) == APR_SUCCESS) {
        ap_rprintf(r, "%s", buffer);
    }
    return APR_SUCCESS;
}

/* The sample content handler */
static int mime_static_html_handler(request_rec * r)
{
    apr_status_t retval;
    const char *html = apr_table_get(table, r->handler);
    if (!html)
        return DECLINED;

    retval = mime_static_html_send_error_document(r, html);
    if (retval == APR_SUCCESS) {
        return OK;
    }
    else {
        return DECLINED;
    }
}

static const char *mime_sattic_html_add_static(cmd_parms * cmd,
                                               void *dummy,
                                               const char *key,
                                               const char *value)
{
    const char *mime;
    const char *html;
    const char *abs_path;

    if (!table)
        table = apr_table_make(cmd->pool, 1);

    mime = apr_pstrdup(cmd->pool, key);

    abs_path = ap_server_root_relative(cmd->pool, value);
    html = (const char *) apr_pstrdup(cmd->pool, abs_path);
    apr_table_set(table, mime, html);

    return NULL;
}

static void mime_static_html_register_hooks(apr_pool_t * p)
{
    ap_hook_handler(mime_static_html_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

const command_rec mime_static_html_cmds[] = {
    AP_INIT_TAKE2("AddMIMEStatic", mime_sattic_html_add_static, NULL,
                  OR_OPTIONS,
                  "specity path to HTML source"),
    {NULL}
};

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA mime_static_html_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                       /* create per-dir    config structures */
    NULL,                       /* merge  per-dir    config structures */
    NULL,                       /* create per-server config structures */
    NULL,                       /* merge  per-server config structures */
    mime_static_html_cmds,      /* table of config file commands       */
    mime_static_html_register_hooks     /* register hooks                      */
};

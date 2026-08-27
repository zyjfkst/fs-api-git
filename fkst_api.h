#ifndef FKST_API_H
#define FKST_API_H

#include <stdint.h>
#include <curl/curl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 错误码 ---- */
#define FKST_OK         0
#define FKST_ERR_NET   -1
#define FKST_ERR_JSON  -2
#define FKST_ERR_AUTH  -3
#define FKST_ERR_INVAL -4
#define FKST_ERR_HTTP  -5
#define FKST_ERR_WS    -6

/* ---- 响应结构 ---- */
typedef struct {
    int code;           // 业务 res 字段（0 成功）
    char *msg;          // 错误信息或 res 的 msg
    char *raw_json;     // 原始响应 JSON 字符串（调用者需释放）
    void *_internal;    // 保留
} FkstResponse;

/* ---- 会话结构（完全定义） ---- */
typedef struct FkstSession {
    char *session_file;
    CURL *curl;
    struct curl_slist *headers;
    char *cookie_jar;          // 未使用，保留

    /* 账户字段 */
    char *mid;
    char *unionid;
    char *openid;
    char *token_seed;
    char *login_token;
    char *device_token;
    char *phone_number;
    char *nickname;

    /* WS 上下文（不透明） */
    void *ws_ctx;
    int ws_running;
} FkstSession;

/* ---- WebSocket 回调 ---- */
typedef void (*FkstWsCallback)(const char *other_mid,
                               const char *letter_id,
                               int type,
                               void *user_data);

/* ---- 常量（需要根据实际情况调整） ---- */
#define FKST_H5_BASE "https://www.yaerxing.com/h5/api"
#define FKST_API_KEY "f42514h"   // 示例，实际可能不同

/* ---- 会话管理 ---- */
FkstSession* fkst_session_new(const char *session_file);
void         fkst_session_free(FkstSession *s);
int          fkst_set_account(FkstSession *s, const char *json_snapshot);
int          fkst_save_session(FkstSession *s);
int          fkst_clear_session(FkstSession *s);

/* ---- 登录 ---- */
FkstResponse* fkst_login_by_password(FkstSession *s, const char *phone, const char *pwd);
FkstResponse* fkst_login_by_code(FkstSession *s, const char *phone, const char *code);
FkstResponse* fkst_login_by_cache(FkstSession *s);
FkstResponse* fkst_logout(FkstSession *s);

/* ---- 快闪卡 ---- */
FkstResponse* fkst_get_fast_cards(FkstSession *s, long long last_time);

/* ---- 我的试卷 ---- */
FkstResponse* fkst_get_my_papers(FkstSession *s, const char *home_id,
                                 const char *keyword, int page);

/* ---- 练习记录 ---- */
FkstResponse* fkst_get_record_questions(FkstSession *s, int page);
FkstResponse* fkst_update_paper_password(FkstSession *s, const char *pid,
                                         const char *type, const char *password);
FkstResponse* fkst_submit_answer(FkstSession *s, const char *aid,
                                 const char *draw_content_json);

/* ---- 私信 REST ---- */
FkstResponse* fkst_get_linkman_list(FkstSession *s, long long updated_at, int count);
FkstResponse* fkst_get_letter_records(FkstSession *s, const char *other_mid,
                                      const char *letter_id, int flag,
                                      const char *last_letter_id);
FkstResponse* fkst_send_letter(FkstSession *s, const char *other_mid,
                               const char *content);

/* ---- 私信 WS ---- */
int  fkst_ws_start(FkstSession *s, const char *other_mid,
                   FkstWsCallback cb, void *user);
void fkst_ws_stop(FkstSession *s);
int  fkst_ws_running(FkstSession *s);

/* ---- 发现流 ---- */
FkstResponse* fkst_get_discover_notes(FkstSession *s, int page,
                                      const char *start_time);
FkstResponse* fkst_like_note(FkstSession *s, const char *note_id, int status);
FkstResponse* fkst_collect_note(FkstSession *s, const char *note_id, int status);

/* ---- 搜索 ---- */
FkstResponse* fkst_search_notes(FkstSession *s, const char *keyword, int page);

/* ---- 收藏 ---- */
FkstResponse* fkst_get_collection(FkstSession *s, int page);

/* ---- 我的资料 ---- */
FkstResponse* fkst_get_my_data(FkstSession *s);

/* ---- 通知设置 ---- */
FkstResponse* fkst_get_notice_config(FkstSession *s);
FkstResponse* fkst_update_notice_config(FkstSession *s, const char *json_toggles);

/* ---- 通用 H5 POST ---- */
FkstResponse* fkst_h5_post(FkstSession *s, const char *api_name,
                           const char *json_data, int sign_all);

/* ---- 辅助 ---- */
void fkst_response_free(FkstResponse *r);
const char* fkst_session_get_mid(FkstSession *s);

#ifdef __cplusplus
}
#endif

#endif // FKST_API_H

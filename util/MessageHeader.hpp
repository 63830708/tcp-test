#ifndef MessageHeaderhpp_H
#define MessageHeaderhpp_H


enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_USER_JOIN,
    CMD_ERROR
};
struct DataHeader
{
    int len;
    int cmd;
};
struct Login: public DataHeader
{
    Login()
    {
        len = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32];
    char password[32];
};
struct LoginResult: public DataHeader
{
    LoginResult(int ret=0)
    {
        len = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = ret;
    }
    int result;
};
struct Logout: public DataHeader
{
    Logout()
    {
            len = sizeof(Logout);
            cmd = CMD_LOGOUT;
    }
    char useName[32];
};
struct LogoutResult: public DataHeader
{
    LogoutResult(int ret=0)
    {
            len = sizeof(LogoutResult);
            cmd = CMD_LOGOUT_RESULT;
            result = ret;
    }
    int result;
};

struct NewUserJoin: public DataHeader
{
    NewUserJoin(int s=0)
    {
        len = sizeof(NewUserJoin);
        cmd = CMD_NEW_USER_JOIN;
        scok = s;
    }
    int scok;
};

#endif

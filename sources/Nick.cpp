#include "../includes/Client.hpp"

void Client::Nick(std::ostringstream &oss)
{
    oss << CYAN BOLT << "NICK <nickname>" RESEND;
    oss << WHITE ITALICS << "\t(Needed to join the server)" RESEND;
    oss << WHITE << "\tSet a new nickname" RESEND;
    oss << WHITE << "\tYour nickname should start with a letter and" RESEND;
    oss << WHITE << "\tcan only have numbers, letters, hyphen (-) and underscore (_)" RESEND;
}

bool Client::NickAlreadyExist(std::string str)
{
    std::vector<std::string> list = _server.GetNickList();
    if (std::find(list.begin(), list.end(), str) != list.end())
    {
        return true;
    }
    return false;
}

/*
 * NICK <nick>
 */
int Client::Nick()
{
    // if PASS not setted yet
    if (_pass_check == false)
    {
        return ERR_NOT_AUTHENTICATED;
    }
    // if NICK doesn't has arguments
    if (_arguments.size() < 1)
    {
        return ERR_NO_NICKNAME_GIVEN;
    }
    // if NICK has too many argument or if NICK isn't valid
    if (_arguments.size() > 1 || ValidName(_arguments[0]) == false)
    {
        return ERR_ERRONEUS_NICKNAME;
    }
    // if NICK isn't unique
    if (NickAlreadyExist(_arguments[0]) == true)
    {
        return ERR_NICKNAME_IN_USE;
    }

    // set NICK
    _server.SetElementNickList(_nick, _arguments[0]);
    _nick = _arguments[0];
    _nick_check = true;

    return NICKNAME_SUCCESS;
}
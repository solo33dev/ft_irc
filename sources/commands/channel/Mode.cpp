#include "../../../includes/Client.hpp"
#include "../../../includes/Channel.hpp"

void Client::mode(std::ostringstream &oss)
{
    oss << CYAN << "├─ " << CYAN BOLT << "MODE <channel> <modes> [params]" << std::endl;
    oss << CYAN << "│" << WHITE << "\tChange channel's mode" << std::endl;
    oss << CYAN << "│" << WHITE << "\tModes:" << std::endl;
    oss << CYAN << "│" << WHITE << "\t  i - Invite-only" << std::endl;
    oss << CYAN << "│" << WHITE << "\t  t - Only ops can change topic" << std::endl;
    oss << CYAN << "│" << WHITE << "\t  k - Set/remove channel key (password)" << std::endl;
    oss << CYAN << "│" << WHITE << "\t  l - Set/remove user limit" << std::endl;
    oss << CYAN << "│" << WHITE << "\t  o - Give/take operator status (needs param)" << std::endl;
}

int Client::mode()
{
    if (_authenticated_check == false)
        return ERR_NOT_AUTHENTICATED;

    if (_arguments.size() < 2)
        return ERR_NEED_MORE_PARAMS;

    std::string channel_name = _arguments[0];
    std::string mode_flags = _arguments[1];
    std::string param = _arguments.size() >= 3 ? _arguments[2] : "";

    std::cout << "[DEBUG] Mode Flags: " << mode_flags << std::endl;  // Debug print


    if (channel_name[0] != '#')
        return ERR_BAD_CHAN_MASK;

    if (!channelExist(channel_name))
        return ERR_NO_SUCH_CHANNEL;

    Channel* channel = _server.getChannelByName(channel_name);
    if (!channel->isMember(this))
        return ERR_NOT_ON_CHANNEL;

    if (!channel->isOperator(this))
        return ERR_CHAN_OP_PRIV_NEEDED;

    bool adding = true;
    size_t paramIndex = 2;
    std::vector<std::string> params;
    std::string mode_changes;

    for (size_t i = 0; i < mode_flags.size(); ++i)
    {
        char mode = mode_flags[i];

        std::cout << "[DEBUG] Processing Mode: " << mode << std::endl;  // Debug print


        if (mode == '+')
        {
            adding = true;
            continue;
        }
        else if (mode == '-')
        {
            adding = false;
            continue;
        }

        switch (mode)
        {
            case 'i':
                channel->setInviteOnly(adding);
				mode_changes += "i";
                break;

            case 't':
        		if (adding) {
        		    channel->setTopicRestriction(true);
        		    mode_changes += "t";
        		}
				else {
        		    channel->setTopicRestriction(false);
        		    mode_changes += "t";
        		}
        		break;

            case 'k':
                if (adding)
                {
                    if (_arguments.size() <= paramIndex)
                        return ERR_NEED_MORE_PARAMS;
                    channel->setPassword(_arguments[paramIndex++]);
					params.push_back(_arguments[paramIndex++]);
                }
                else {
					channel->removePassword();
					mode_changes += "k";
				}
				mode_changes += "k";
                break;

            case 'l':
                if (adding)
                {
                    if (_arguments.size() <= paramIndex)
                        return ERR_NEED_MORE_PARAMS;
                    int limit = std::atoi(_arguments[paramIndex++].c_str());
                    channel->setUserLimit(limit);
					params.push_back(_arguments[paramIndex++]);
                }
                else {
					channel->removeUserLimit();
					mode_changes += "l";
				}
				mode_changes += "l";
                break;

            case 'o':
			{
                if (_arguments.size() <= paramIndex)
                    return ERR_NEED_MORE_PARAMS;

                Client* target = _server.getClientByNick(_arguments[paramIndex++]);
                if (!target || !channel->isMember(target))
                    return ERR_NOT_ON_CHANNEL;
                if (adding)
                    channel->addOperator(target);
                else
                    channel->removeOperator(target);
				 mode_changes += "o";
                params.push_back(_arguments[paramIndex++]);
                break;
			}

            default:
                return ERR_UNKNOWN_MODE;
        }
    }

	// Send the mode change message to the channel
	std::string full_mask = getFullMask();
    std::string mode_message = ":" + full_mask + " MODE " + channel_name + " " + mode_flags;

	// If there are parameters (for example, a password or user limit), append them
    if (!param.empty())
        mode_message += " " + param;

    // Broadcast the mode change message to all channel members
    channel->broadcast(mode_message + "\r\n");

    // Print message indicating the mode has been updated
    printMessage(MODE_UPDATED);
    return 0;
}

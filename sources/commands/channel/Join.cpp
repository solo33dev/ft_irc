#include "../../../includes/Client.hpp"
#include "../../../includes/Channel.hpp"

void Client::join(std::ostringstream &oss)
{
    oss << CYAN << "├─ " << CYAN BOLT << "JOIN <channel>" << std::endl;
    oss << CYAN << "│" << WHITE << "\tJoin a channel" << std::endl;
    oss << CYAN << "│" << WHITE << "\tIf channel doesn't exist:" << std::endl;
    oss << CYAN << "│" << WHITE << "\t- Create a new one" << std::endl;
    oss << CYAN << "│" << WHITE << "\t- Become de channel operator" << std::endl;
}

int Client::join()
{
	if (!_authenticated_check)
		return ERR_NOT_AUTHENTICATED;

	if (_arguments.size() != 1 && _arguments.size() != 2)
		return ERR_NEED_MORE_PARAMS;

	std::string channel_name = _arguments[0];
	if (channel_name.empty() || channel_name[0] != '#')
		return ERR_BAD_CHAN_MASK;

	Channel* channel = NULL;

	if (channelExist(channel_name)) {
		channel = _server.getChannelByName(channel_name);

		if (channel->isMember(this))
			return ERR_USER_ON_CHANNEL;
		
		if (!channel->getPassword().empty())
		{
			// Check if password argument is present
			if (_arguments.size() < 2)
				return ERR_BAD_CHANNEL_KEY;

			std::string pass = _arguments[1];
			if (pass != channel->getPassword())
				return ERR_BAD_CHANNEL_KEY;
		}

		if (!channel->isInvited(this) && (channel->isInviteOnly() || channel->isKicked(this)))
			return ERR_INVITE_ONLY_CHAN;
		
		if (channel->getUserLimit() > 0 && channel->getUserCount() >= channel->getUserLimit())
			return ERR_CHANNEL_IS_FULL;		

		channel->addClient(this);

		channel->removeInvited(this);

		sendJoinMessages(channel_name, channel);
		printMessage(JOINED_CHANNEL);
	} else {
		_server.setNewChannel(channel_name);

		const std::vector<Channel>& channels = _server.getChannelList();
		for (size_t i = 0; i < channels.size(); ++i) {
			if (channels[i].getName() == channel_name) {
				channel = const_cast<Channel*>(&channels[i]);
				break;
			}
		}

		if (channel) {
			channel->addClient(this);
			channel->addOperator(this);
			sendJoinMessages(channel_name, channel);
			printMessage(CHANNEL_CREATED);
			printMessage(JOINED_CHANNEL);
			printMessage(CHANNEL_OP);
		}
	}
	return 0;
}

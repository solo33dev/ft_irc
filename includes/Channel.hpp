#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "format/text_format.h"
# include "format/config_macros.h"
# include "../includes/Client.hpp"
# include <map>

class Channel
{
private:
	typedef std::map<std::string, Client *> _mapclient;

	_mapclient		clients;
	_mapclient		invited;
	std::vector<Client*>	operators;
	std::string			mode;
	std::string			name;
	std::string			key;
	std::string			topic;
	std::string			password;
	int					usersLimit;
	int					usersCount;

public:
	Channel(std::string);
	~Channel();

	// METHODS

	// GETTERS
	std::string getName() const;

private:
	Channel();

	// ATTRIBUTES
	std::string _name;

	// Client management
	void			addClient(Client *client);
	bool			isMember(Client *client);
	void			removeClient(Client *client);
	bool			isEmpty(void);
	
	// Messaging
	void			sendMessage(const std::string &source, const std::string &command, const std::string &args);
	void			privateMessage(const std::string &source, const std::string &command, const std::string &args);
	
	// Invitation
	bool			isInvited(Client *client);
	void			addInvited(Client *client);
	void			delInvited(Client *client);
	
	// Mode management
	std::string		addMode(const std::string &mode);
	std::string		delMode(const std::string &mode);
	std::string		getPrefix(Client *client) const;
	std::string		getSymbol(void) const;

	// Operator methods
	bool			isOperator(Client *client) const;
	bool			addOperator(Client* client);
	bool			removeOperator(Client* client);

	// Password methods
	void			setPassword(const std::string& password);
	const std::string& getPassword() const;
	void			removePassword();

	// Getters and setters
	const std::string		&getName(void) const;
	void						setName(const std::string &src);
	const _mapclient	&getClients(void) const;
	void						setClients(_mapclient &src);
	const std::string		&getKey(void) const;
	void						setKey(std::string &src);
	const std::string		&getTopic(void) const;
	void						setTopic(const std::string &src);
	void						setInvited(_mapclient &src);
	const _mapclient	&getInvited(void) const;
	const std::string		getMode(void) const;
	int 						getUserLimit() const;
	void						setUserLimit(int limit);
	void 						removeUserLimit();
	int 						getUserCount() const;
	void						setUserCount(int count);
};

std::ostream &operator<<(std::ostream &o, const Channel &src);

#endif

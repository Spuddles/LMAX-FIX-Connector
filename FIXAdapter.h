#pragma once
#include <string>
#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/SocketInitiator.h"
#include "FIXOrderCollection.h"
#include "CurrencyCollection.h"

using namespace FIX;

class CFIXAdapter : public FIX::Application,
					public FIX::MessageCracker
{
private:
	FIX::SocketInitiator		*m_pInitiator;

	unsigned int				m_OrderCount;
	FIX::SessionID				m_TradeSessionID;
	FIX::SessionID				m_PriceSessionID;

	std::string					m_UserName;
	std::string					m_Password;

	std::string					m_TradeCompID;
	std::string					m_PriceCompID;

	bool						m_bTradeLoggedIn;
	bool						m_bPriceLoggedIn;

	std::map<int, std::string>	m_mapMarketData;

	// Stores all the orders we have received
	CFIXOrderCollection			m_Orders;

	// Stores all the tradable currencies
	CCurrencyCollection			m_Currencies;

	std::string		GenerateClOrdID();

	// Notification of a session begin created
	virtual void onCreate( const SessionID& );

	// These are all the message types we can get back from the server
	void	onMessage(const FIX44::Heartbeat &message, const FIX::SessionID&);
	void	onMessage(const FIX44::Reject &message, const FIX::SessionID&);
	void	onMessage(const FIX44::Logout &message, const FIX::SessionID&);
	void	onMessage(const FIX44::ExecutionReport &message, const FIX::SessionID&);
	void	onMessage(const FIX44::OrderCancelReject &message, const FIX::SessionID&);
	void	onMessage(const FIX44::Logon &message, const FIX::SessionID&);
	void	onMessage(const FIX44::MarketDataSnapshotFullRefresh &message, const FIX::SessionID&);

	void	handleNewOrder(const FIX44::ExecutionReport &message);
	void	handlePartiallyFilledOrder(const FIX44::ExecutionReport &message);
	void	handleFilledOrder(const FIX44::ExecutionReport &message);
	void	handleCanceledOrder(const FIX44::ExecutionReport &message);
	void	handleRejectedOrder(const FIX44::ExecutionReport &message);

	// Notification of a session successfully logging on
	virtual void onLogon( const SessionID& );
	// Notification of a session logging off or disconnecting
	virtual void onLogout( const SessionID& );
	// Notification of admin message being sent to target
	virtual void toAdmin( Message&, const SessionID& );
	// Notification of app message being sent to target
	virtual void toApp( Message&, const SessionID& ) throw( DoNotSend );
	// Notification of admin message being received from target
	virtual void fromAdmin( const Message&, const SessionID& ) 
		throw( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, RejectLogon );
	// Notification of app message being received from target
	virtual void fromApp( const Message&, const SessionID& ) 
		throw( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, UnsupportedMessageType );

public:
			CFIXAdapter();
			~CFIXAdapter();

	// Alow the client to set the login details
	void	SetLoginDetails(const std::string &UserName,
						    const std::string &Password);

	void	SetCompIDs(const std::string &TradeCompID,
					   const std::string &PriceCompID);

	bool	Initialise();
	bool	Shutdown();

	// Check to see if we are currently logged in
	bool	IsTradeLoggedIn();
	bool	IsPriceLoggedIn();

	// These are all the messages we can send to the server
	bool	TestRequest(const std::string &RequestID);
	bool	SendSequenceReset();
	bool	Logout(const std::string &Reason);
	bool	NewOrderSingle(const std::string &Currency, double Quantity, double Price, bool bIsLimit, std::string &OrderID);
	bool	OrderCancelRequest(const std::string &OrigClOrdID, const std::string &Currency, double Quantity);

	bool	StartMarketDataRequest(const std::string &Currency);
	bool	DisableMarketDataRequest(const std::string &Currency);
};


#include "stdafx.h"
#include "Platform.h"
#include "TimeObject.h"
#include "FIXAdapter.h"
#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/fix44/Logon.h"
#include "quickfix/fix44/Logout.h"
#include "quickfix/fix44/TestRequest.h"
#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix44/OrderCancelRequest.h"
#include "quickfix/fix44/SequenceReset.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"

using namespace std;
using namespace FIX;

CFIXAdapter::CFIXAdapter(void):
			m_pInitiator(nullptr),
			m_OrderCount(0),
			m_bTradeLoggedIn(false),
			m_bPriceLoggedIn(false)
{
}

CFIXAdapter::~CFIXAdapter(void)
{
}

void CFIXAdapter::SetLoginDetails(const string &UserName,
								  const string &Password)
{
	m_UserName = UserName;
	m_Password = Password;
}

void CFIXAdapter::SetCompIDs(const std::string &TradeCompID,
							 const std::string &PriceCompID)
{
	m_TradeCompID = TradeCompID;
	m_PriceCompID = PriceCompID;
}

bool CFIXAdapter::Initialise()
{
	bool bRet = true;
	try
	{
		// Assume that the config file is in the same directory as the executable
		FIX::SessionSettings	settings("fixsettings.cfg");
		FIX::FileStoreFactory	storeFactory(settings);
		FIX::FileLogFactory		logFactory(settings);
		m_pInitiator = new SocketInitiator(*this, storeFactory, settings, logFactory);

		// Start the connection and login process
		m_pInitiator->start();

		// Setup all the currency details
		if (!m_Currencies.Initialise())
		{
			cout << "Failed to initialise the currency collection" << endl;
			return false;
		}
	}
	catch(const FIX::ConfigError &error)
	{
		cout << error.detail << endl;
	}
	catch(...)
	{
		bRet = false;
	}
	return bRet;
}

bool CFIXAdapter::Shutdown()
{
	bool bRet = true;
	try
	{
		if (m_pInitiator)
		{
			m_pInitiator->stop();

			while (!m_pInitiator->isStopped())
			{
				Sleep(100);
			}
//			delete m_pInitiator;
		}
	}
	catch(...)
	{
		bRet = false;
	}
	return bRet;
}

bool CFIXAdapter::IsTradeLoggedIn()
{
	return m_bTradeLoggedIn;
}

bool CFIXAdapter::IsPriceLoggedIn()
{
	return m_bPriceLoggedIn;
}

void CFIXAdapter::onCreate( const SessionID &sessionID )
{
//	cout << "Session to LMAX has been created successfully" << endl;
}

void CFIXAdapter::onLogon( const SessionID &sessionID )
{
	cout << "We are logged on to the ";

	string CompID = sessionID.getTargetCompID();
	if (CompID.compare(m_TradeCompID)==0)
	{
		cout << "trade server";
		m_bTradeLoggedIn = true;
	}
	else if (CompID.compare(m_PriceCompID)==0)
	{
		cout << "market data server";
		m_bPriceLoggedIn = true;
	}
	else
	{
		cout << "unknown server (" << sessionID.getTargetCompID() << ")";
	}
	cout << endl;
}

void CFIXAdapter::onLogout( const SessionID &sessionID )
{
	cout << "We are logged out from the ";

	string CompID = sessionID.getTargetCompID();
	if (CompID.compare(m_TradeCompID)==0)
	{
		cout << "trade server";
		m_bTradeLoggedIn = false;
	}
	else if (CompID.compare(m_PriceCompID)==0)
	{
		cout << "market data server";
		m_bPriceLoggedIn = false;
	}
	else
	{
		cout << "unknown server";
	}
	cout << endl;
}

void CFIXAdapter::toAdmin( Message &message, const SessionID &sessionID )
{
	cout << message << endl;
	// If this is a login message we need to add the username and password
	string value = message.getHeader().getField(FIX::FIELD::MsgType);
	if (value.compare("A")==0)
	{
		message.getHeader().setField(FIX::Password(m_Password));
		message.getHeader().setField(FIX::Username(m_UserName));
		message.setField(FIX::ResetSeqNumFlag(true));

		// Store the session ID
		string TargetCompID = message.getHeader().getField(FIX::FIELD::TargetCompID);
		if (TargetCompID.compare("LMXBDM")==0)
		{
			m_PriceSessionID = sessionID;
		}
		else if (TargetCompID.compare("LMXBD")==0)
		{
			m_TradeSessionID = sessionID;
		}
	}
}

void CFIXAdapter::toApp( Message &message, const SessionID &sessionID ) 
	throw( DoNotSend )
{
	cout << "toApp:" << message << " - " << sessionID << endl;
}

void CFIXAdapter::fromAdmin( const Message &message, const SessionID &sessionID ) 
	throw( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, RejectLogon )
{
	cout << message << endl;
	crack(message, sessionID);
}

void CFIXAdapter::fromApp( const Message &message, const SessionID &sessionID ) 
	throw( FieldNotFound, IncorrectDataFormat, IncorrectTagValue, UnsupportedMessageType )
{
	try
	{
		cout << message << endl;
		crack(message, sessionID);
	}
	catch(FIX::UnsupportedMessageType &/*value*/)
	{
		cout << "Not handled message " << message << endl;
	}
}

string CFIXAdapter::GenerateClOrdID()
{
	// This is a 16 char hex number without a leading zero
	CTimeObject dtNow = CTimeObject::Now();

	stringstream ss;
	ss << setfill('0');
	ss << setw(4) << dtNow.GetYear();
	ss << setw(2) << dtNow.GetMonth();
	ss << setw(2) << dtNow.GetDay();
	ss << setw(2) << dtNow.GetHour();
	ss << setw(2) << dtNow.GetMinute();
	ss << setw(2) << dtNow.GetSecond();
	ss << setw(2) << (m_OrderCount++) % 100;

	return ss.str();
}

/*****************************************************************************/
/*          All the available client functions that can be used				 */
/*                  to issue commands to the FIX server						 */
/*****************************************************************************/
bool CFIXAdapter::TestRequest(const string &RequestID)
{
	bool bRet = false;

	if (m_bTradeLoggedIn)
	{
		FIX44::TestRequest message;

		message.set(FIX::TestReqID(RequestID));

		cout << "Sending test request" << endl;
		bRet = FIX::Session::sendToTarget(message, m_TradeSessionID);
	}
	return bRet;
}

bool CFIXAdapter::SendSequenceReset()
{
	bool bRet = false;

	if (m_bTradeLoggedIn)
	{
		FIX44::SequenceReset message;

		message.set(FIX::GapFillFlag(false));

		bRet = FIX::Session::sendToTarget(message, m_TradeSessionID);
	}
	return bRet;
}

bool CFIXAdapter::Logout(const string &Reason)
{
	bool bRet = false;

	if (m_bTradeLoggedIn)
	{
		FIX44::Logout message;
		message.set(FIX::Text(Reason));

		bRet = FIX::Session::sendToTarget(message);
	}
	return bRet;
}

bool CFIXAdapter::NewOrderSingle(const string &Currency, double Quantity, double Price, bool bIsLimit, string &OrderID)
{
	if (!m_bTradeLoggedIn)
	{
		cout << "Not logged in so unable to send new order" << endl;
		return false;
	}

	// Get the unique order ID
	string ClOrdID = GenerateClOrdID();

	FIX::Side	OrderSide;
	if (Quantity > 0)
	{
		OrderSide = FIX::Side_BUY;
	}
	else
	{
		OrderSide = FIX::Side_SELL;
	}

	FIX::OrdType OrderType;
	if (bIsLimit)
	{
		OrderType = FIX::OrdType_LIMIT;
	}
	else
	{
		OrderType = FIX::OrdType_MARKET;
	}

	FIX44::NewOrderSingle message(
		FIX::ClOrdID(ClOrdID),
		OrderSide,				
		FIX::TransactTime(true),
		OrderType);		

	// Convert currency string into ID
	int CurrencyID;
	if (!m_Currencies.GetCurrencyID(Currency, CurrencyID))
	{
		cout << "Unable to get the currencyID for " << Currency << endl;
		return false;
	}

	// Instrument
	stringstream ss;
	ss << CurrencyID;
	message.set(FIX::SecurityID(ss.str()));		// LMAX orderbook ID
	message.set(FIX::SecurityIDSource("8"));	// "8" = Exchange Symbol

	// OrderQtyData
	message.set(FIX::OrderQty(fabs(Quantity)));

	message.set(FIX::Price(Price));

	// Time in force is dependant on the order type
	if (bIsLimit)
	{
		// Valid all day (trading session)
		message.set(FIX::TimeInForce('0'));
	}
	else
	{
		// Immediate or Cancel
		message.set(FIX::TimeInForce('3'));
	}

	return FIX::Session::sendToTarget(message, m_TradeSessionID);
}

bool CFIXAdapter::OrderCancelRequest(const string &OrigClOrdID, const string &Currency, double Quantity)
{
	if (!m_bTradeLoggedIn)
	{
		return false;
	}

	string ClOrdID = GenerateClOrdID();

	FIX::Side OrderSide;
	if (Quantity > 0)
	{
		OrderSide = FIX::Side_BUY;
	}
	else
	{
		OrderSide = FIX::Side_SELL;
	}

	FIX44::OrderCancelRequest message(
		FIX::OrigClOrdID(OrigClOrdID),
		FIX::ClOrdID(ClOrdID),
		OrderSide,
		FIX::TransactTime(true));

	// Convert currency string into ID
	int CurrencyID;
	if (!m_Currencies.GetCurrencyID(Currency, CurrencyID))
	{
		cout << "Unable to get the currencyID for " << Currency << endl;
		return false;
	}

	// Instrument
	stringstream ss;
	ss << CurrencyID;
	message.set(FIX::SecurityID(ss.str()));		// LMAX orderbook ID
	message.set(FIX::SecurityIDSource("8"));	// "8" = Exchange Symbol

	// OrderQtyData
	message.set(FIX::OrderQty(fabs(Quantity)));

	return FIX::Session::sendToTarget(message, m_TradeSessionID);
}

/******************************************************************************/
/*              Handler functions for every type of message					  */
/*                  we can get back from the FIX server						  */
/******************************************************************************/
void CFIXAdapter::onMessage(const FIX44::Heartbeat &message, const FIX::SessionID&)
{
	cout << "We have received a heartbeat message" << endl;
}

void CFIXAdapter::onMessage(const FIX44::Reject &message, const FIX::SessionID&)
{
	cout << "We have received a reject message" << endl;
}

void CFIXAdapter::onMessage(const FIX44::Logout &message, const FIX::SessionID&)
{
	cout << "We have received a logout message" << endl;
}

void CFIXAdapter::onMessage(const FIX44::ExecutionReport &message, const FIX::SessionID&)
{
	cout << "We have received an execution report" << endl;

	// Check to see what sort of message this is
	FIX::OrdStatus orderStatus;
	message.get(orderStatus);

	switch(orderStatus)
	{
	case	'0':	handleNewOrder(message);
					break;
	case	'1':	handlePartiallyFilledOrder(message);
					break;
	case	'2':	handleFilledOrder(message);
					break;
	case	'4':	handleCanceledOrder(message);
					break;
	case	'8':	handleRejectedOrder(message);
					break;
	default:		cout << "Unknown ExecutionReport with status " << orderStatus << endl;
	}
}

void CFIXAdapter::onMessage(const FIX44::OrderCancelReject &message, const FIX::SessionID&)
{
	cout << "We have received a cancel order reject message" << endl;
}

void CFIXAdapter::onMessage(const FIX44::Logon &message, const FIX::SessionID&)
{
	cout << "We have received a login message" << endl;
}

void CFIXAdapter::onMessage(const FIX44::MarketDataSnapshotFullRefresh &message, const FIX::SessionID&)
{
	cout << "Quote received" << endl;

	FIX::SecurityID										SecID;
	FIX::NoMDEntries									NoValues;
	FIX44::MarketDataSnapshotFullRefresh::NoMDEntries	Group;
	FIX::MDEntryType									QuoteType;
	FIX::MDEntryPx										QuotePrice;
	FIX::MDEntrySize									QuoteSize;
	FIX::MDEntryDate									QuoteDate;
	FIX::MDEntryTime									QuoteTime;

	try
	{
		// Get the currency ID
		message.get(SecID);
		string CurrencyID = SecID;

		// Get number of values
		message.get(NoValues);
		int Count = NoValues;

		for (int i=1;i<=Count;i++)
		{
			message.getGroup(i, Group);
			Group.get(QuoteType);
			Group.get(QuotePrice);
			Group.get(QuoteSize);
			// Date and time are the same for all items in this group
			if (i==1)
			{
				Group.get(QuoteDate);
				Group.get(QuoteTime);
		
			   CTimeObject now = CTimeObject::Now();

				// Create message string
				cout << setfill('0');
				cout << setw(2) << now.GetHour() << ":";
				cout << setw(2) << now.GetMinute() << ":";
				cout << setw(2) << now.GetSecond() << ".";
				cout << setw(3) << now.GetMillisecond();

				cout << QuoteTime << endl;

//				cout << CurrencyID << ":" << QuoteType << " " << QuotePrice << " " << QuoteSize << " ";
//				cout << QuoteDate << " " << QuoteTime << endl;
			}
		}
	}
	catch(...)
	{
		// Don't know what to do here
	}
}

/*void CFIXAdapter::HandleResendRequest(const Message &message)
{
	cout << "We have received a resend request" << endl;

	string StartSeqStr = message.getField(FIX::FIELD::BeginSeqNo);
	string EndSeqStr = message.getField(FIX::FIELD::EndSeqNo);
//	cout << "Asking for messages " << StartSeqStr << " to " << EndSeqStr << endl;

//	SendSequenceReset();
}*/

/*
 * Create a new order in our system keyed on the ClOrdID
 */
void CFIXAdapter::handleNewOrder(const FIX44::ExecutionReport &message)
{
	// Grab the ClOrdID
	FIX::ClOrdID clOrdID;
	message.get(clOrdID);

	// Get the amount we have requested
	FIX::OrderQty orderQty;
	message.get(orderQty);

	// Get the order side
	FIX::Side side;
	message.get(side);

	if (side == FIX::Side_BUY)
	{
		// Update our trades collection
		m_Orders.Add(clOrdID, orderQty, eOrderSide::BUY);
	}
	else
	{
		// Update our trades collection
		m_Orders.Add(clOrdID, orderQty, eOrderSide::SELL);
	}
}

/*
 * Update our previously created order with partial fill information
 */
void CFIXAdapter::handlePartiallyFilledOrder(const FIX44::ExecutionReport &message)
{
	// Grab the ClOrdID
	FIX::ClOrdID clOrdID;
	message.get(clOrdID);

	// Get the new total amount of fills
//	FIX::
	double Received=0.0;

	m_Orders.Update(clOrdID, Received);
}

/*
 * Update our order and mark it as successful and complete
 */
void CFIXAdapter::handleFilledOrder(const FIX44::ExecutionReport &message)
{
	// Grab the ClOrdID
	FIX::ClOrdID clOrdID;
	message.get(clOrdID);

	double Received=0.0;
	m_Orders.Update(clOrdID, Received);
	m_Orders.Update(clOrdID, eOrderState::FILLED);
}

/*
 * Update out order and mark it as cancelled and complete
 */
void CFIXAdapter::handleCanceledOrder(const FIX44::ExecutionReport &message)
{
	// Grab the ClOrdID
	FIX::ClOrdID clOrdID;
	message.get(clOrdID);

	m_Orders.Update(clOrdID, eOrderState::CANCELED);
}

/*
 * Update our order and mark it as rejected and complete
 */
void CFIXAdapter::handleRejectedOrder(const FIX44::ExecutionReport &message)
{
	// Grab the ClOrdID
	FIX::ClOrdID clOrdID;
	message.get(clOrdID);

	m_Orders.Update(clOrdID, eOrderState::REJECTED);
}

bool CFIXAdapter::StartMarketDataRequest(const string &Currency)
{
	cout << "Subscribing to currency " << Currency << endl;

	int CurrencyID;
	if (!m_Currencies.GetCurrencyID(Currency, CurrencyID))
	{
		cout << "Unable to get currency for ID " << CurrencyID << endl;
		return false;
	}

	// Make sure we are not already subscribed to this one
	if (m_mapMarketData.find(CurrencyID)!=m_mapMarketData.end())
	{
		cout << "We are already subscribed to market data for currencyID " << CurrencyID << endl;
		return false;
	}

	// Get the unique order ID
	string RequestID = GenerateClOrdID();

	// Register this as subscribed
	m_mapMarketData[CurrencyID] = RequestID;

	FIX44::MarketDataRequest message(
		FIX::MDReqID(RequestID),
		FIX::SubscriptionRequestType(SubscriptionRequestType_SNAPSHOT_PLUS_UPDATES),				
		FIX::MarketDepth(0));		

	FIX44::MarketDataRequest::NoMDEntryTypes group1;
	group1.set(FIX::MDEntryType(FIX::MDEntryType_BID));
	message.addGroup(group1);
	group1.set(FIX::MDEntryType(FIX::MDEntryType_OFFER));
	message.addGroup(group1);

	// Instrument
	stringstream ss;
	ss << CurrencyID;

	FIX44::MarketDataRequest::NoRelatedSym group2;
	group2.set(FIX::SecurityID(ss.str()));
	group2.set(FIX::SecurityIDSource("8"));
	message.addGroup(group2);

	bool bRet = true;

	try
	{
		bRet = FIX::Session::sendToTarget(message, m_PriceSessionID);
	}
	catch(FIX::SessionNotFound &SNF)
	{
		cout << SNF.detail << endl;
		bRet = false;
	}
	return bRet;
}

bool CFIXAdapter::DisableMarketDataRequest(const string &Currency)
{
	cout << "Unsubscribing from currency " << Currency << endl;

	int CurrencyID;
	if (!m_Currencies.GetCurrencyID(Currency, CurrencyID))
	{
		cout << "Unable to get currency ID for " << Currency << endl;
		return false;
	}

	// See if we are already subscribed to this currency
	auto it = m_mapMarketData.find(CurrencyID);
	if (it == m_mapMarketData.end())
	{
		cout << "We are trying to unsubscribe from a currency we are not subscribed to (" << CurrencyID << ")" << endl;
		return false;
	}

	FIX44::MarketDataRequest message(
		FIX::MDReqID(it->second),
		FIX::SubscriptionRequestType(SubscriptionRequestType_UNSUBSCRIBE),				
		FIX::MarketDepth(0));		

	m_mapMarketData.erase(it);

	FIX44::MarketDataRequest::NoMDEntryTypes group1;
	group1.set(FIX::MDEntryType(FIX::MDEntryType_BID));
	message.addGroup(group1);
	group1.set(FIX::MDEntryType(FIX::MDEntryType_OFFER));
	message.addGroup(group1);

	// Instrument
	stringstream ss;
	ss << CurrencyID;

	FIX44::MarketDataRequest::NoRelatedSym group2;
	group2.set(FIX::SecurityID(ss.str()));
	group2.set(FIX::SecurityIDSource("8"));
	message.addGroup(group2);

	bool bRet = true;

	try
	{
		bRet = FIX::Session::sendToTarget(message, m_PriceSessionID);
	}
	catch(FIX::SessionNotFound &SNF)
	{
		cout << SNF.detail << endl;
		bRet = false;
	}
	return bRet;
}

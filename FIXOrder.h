#pragma once
#include <string>
#include <time.h>

enum eOrderState
{
	NEW = 1,
	PARTIALLY_FILLED,
	FILLED,
	REJECTED,
	CANCELED
};

enum eOrderSide
{
	BUY = 1,
	SELL
};

class CFIXOrder
{
	std::string		m_OrderID;
	double			m_AmountRequested;
	double			m_AmountReceived;
	eOrderState		m_OrderState;
	eOrderSide		m_Side;
	tm				m_CreationTime;
	tm				m_LastUpdatedTime;

public:
				CFIXOrder(void);
				CFIXOrder(std::string OrderID);
				~CFIXOrder(void);

	void		SetAmountRequested(double Amount);
	double		GetAmountRequested();

	void		SetAmountReceived(double Amount);
	double		GetAmountReceived();

	void		SetOrderState(eOrderState State);
	eOrderState	GetOrderState();

	void		SetOrderSide(eOrderSide Side);
	eOrderSide	GetOrderSide();
};


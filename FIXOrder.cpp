#include "stdafx.h"
#include "FIXOrder.h"

using namespace std;

CFIXOrder::CFIXOrder(void)
{
}

CFIXOrder::CFIXOrder(string OrderID)
{
	m_OrderID = OrderID;
}

CFIXOrder::~CFIXOrder(void)
{
}

void CFIXOrder::SetAmountRequested(double Amount)
{
	m_AmountRequested = Amount;
}

double CFIXOrder::GetAmountRequested()
{
	return m_AmountRequested;
}

void CFIXOrder::SetAmountReceived(double Amount)
{
	m_AmountReceived = Amount;
}

double CFIXOrder::GetAmountReceived()
{
	return m_AmountReceived;
}

void CFIXOrder::SetOrderState(eOrderState State)
{
	m_OrderState = State;
}

eOrderState CFIXOrder::GetOrderState()
{
	return m_OrderState;
}

void CFIXOrder::SetOrderSide(eOrderSide Side)
{
	m_Side = Side;
}

eOrderSide CFIXOrder::GetOrderSide()
{
	return m_Side;
}

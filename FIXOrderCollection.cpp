#include "stdafx.h"
#include "FIXOrderCollection.h"
#include "CSGuardian.h"

using namespace std;

CFIXOrderCollection::CFIXOrderCollection(void)
{
}

CFIXOrderCollection::~CFIXOrderCollection(void)
{
}

bool CFIXOrderCollection::Add(const string &OrderID, double Requested, eOrderSide Side)
{
	CCSGuardian csg(m_CS);

	// If we already have an entry with this OrderID, fail
	if (m_mapOrders.find(OrderID) != m_mapOrders.end())
	{
		return false;
	}

	CFIXOrder	order(OrderID);

	order.SetAmountRequested(Requested);
	order.SetOrderSide(Side);

	m_mapOrders[OrderID] = order;

	return true;
}

bool CFIXOrderCollection::Update(const string &OrderID, double Received)
{
	CCSGuardian csg(m_CS);

	// Look for the matching order
	auto it = m_mapOrders.find(OrderID);
	if (it == m_mapOrders.end())
	{
		// No matching order to update
		return false;
	}

	it->second.SetAmountReceived(Received);

	return true;
}

bool CFIXOrderCollection::Update(const string &OrderID, eOrderState State)
{
	CCSGuardian csg(m_CS);

	// Look for the matching order
	auto it = m_mapOrders.find(OrderID);
	if (it == m_mapOrders.end())
	{
		// No matching order to update
		return false;
	}

	it->second.SetOrderState(State);

	return true;
}

bool CFIXOrderCollection::Delete(const string &OrderID)
{
	CCSGuardian csg(m_CS);

	// Look for the matching order
	auto it = m_mapOrders.find(OrderID);
	if (it == m_mapOrders.end())
	{
		// No matching order to update
		return false;
	}

	m_mapOrders.erase(it);

	return true;
}

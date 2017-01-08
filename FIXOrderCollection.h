#pragma once
#include <map>
#include <string>
#include "FIXOrder.h"
#include "CSGuardian.h"

class CFIXOrderCollection
{
	std::map<std::string, CFIXOrder>	m_mapOrders;
	CRIT_SECTION						m_CS;
public:
			CFIXOrderCollection(void);
			~CFIXOrderCollection(void);

	bool	Add(const std::string &OrderID, double Requested, eOrderSide Side);
	bool	Update(const std::string &OrderID, double Received);
	bool	Update(const std::string &OrderID, eOrderState State);
	bool	Delete(const std::string &OrderID);
};


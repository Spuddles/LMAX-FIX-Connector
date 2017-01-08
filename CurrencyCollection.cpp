#include "stdafx.h"
#include "CurrencyCollection.h"
#include "Tradeables.h"

using namespace std;

CCurrencyCollection::CCurrencyCollection(void)
{
}

CCurrencyCollection::~CCurrencyCollection(void)
{
}

bool CCurrencyCollection::Initialise()
{
	// Make sure the collection class is empty
	m_mapCurrencies.clear();

	// Build up the map with the items keyed on name
	for (int i=0;i<CURRENCY_PAIRS;i++)
	{
		CurrencyPairDetails cpd = Tradeables[i];
		m_mapCurrencies[cpd.Ticker] = cpd;
	}
	return true;
}

bool CCurrencyCollection::GetCurrency(const std::string &Currency, CurrencyPairDetails &cpd)
{
	auto it = m_mapCurrencies.find(Currency);
	if (it != m_mapCurrencies.end())
	{
		cpd = it->second;
		return true;
	}
	return false;
}

bool CCurrencyCollection::GetCurrencyID(const string &Currency, int &CurrencyID)
{
	auto it = m_mapCurrencies.find(Currency);
	if (it != m_mapCurrencies.end())
	{
		CurrencyID = it->second.ID;
		return true;
	}
	return false;
}
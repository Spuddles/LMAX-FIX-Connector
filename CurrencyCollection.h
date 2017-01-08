#pragma once
#include <map>
#include <string>

struct CurrencyPairDetails
{
	std::string	Ticker;
	int			ID;
	int			ContractMultiplier;
	double		TickSize;
	double		TickValue;
};

class CCurrencyCollection
{
private:
	std::map<std::string, CurrencyPairDetails>	m_mapCurrencies;
public:
				CCurrencyCollection(void);
				~CCurrencyCollection(void);

	bool		Initialise();
	bool		GetCurrency(const std::string &Currency, CurrencyPairDetails &cpd);
	bool		GetCurrencyID(const std::string &Currency, int &CurrencyID);
};


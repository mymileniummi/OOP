#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;
int max_int = 10e8;

class shop 
{
private:
	int shop_ID;
	string shop_name;
	string shop_adress;
	map<string, pair<int, int>> product_list;
public:
	shop(int shop_ID_, string shop_name_, string shop_adress_)
		:shop_ID(shop_ID_)
		,shop_name(shop_name_)
		,shop_adress(shop_adress_)
	{}
	void add_new_product(string product_name_, int amount_, int price_) {
		auto cur = product_list.find(product_name_);
		if (cur == product_list.end())
			product_list.emplace(product_name_, make_pair(amount_, price_));
		else
			throw(exception("product already exists"));
	}
	void deliver_new_product_(string product_name_, int amount_, int price_) {
		auto cur_product = product_list.find(product_name_);
		if (cur_product == product_list.end()) {
			add_new_product(product_name_, amount_, price_);
			cur_product = product_list.find(product_name_);
		}
		else {
			cur_product->second.first += amount_;
			cur_product->second.second += price_;
		}
	}
	int get_product_price(string pruduct_name_) {
		auto res = product_list.find(pruduct_name_);
		if (res == product_list.end()) {
			throw (exception("No products with this name"));
		}
		if (res->second.first == 0)
			return max_int;
		else
			return res->second.second;
	}
	int get_product_amount(string pruduct_name_) {
		auto res = product_list.find(pruduct_name_);
		if (res == product_list.end()) {
			throw (exception("No products with this ID"));
		}
		return res->second.first;
	}
	string get_shop_name() {
		return shop_name;
	}
	void show_products() {
		cout << "------======" << shop_name << "======-------\n";
		for (auto cur = product_list.begin(); cur != product_list.end(); cur++) {
			cout << cur->first << " " << cur->second.first <<
				" " << cur->second.second << "\n";
		}
	}
};
void check_shop_existance(int ID_,const map<int, shop> &shop_list_) {
	auto shop = shop_list_.find(ID_);
	if (shop == shop_list_.end()) {
		throw(exception("No shop with this ID"));
	}
}
string find_product_name(int ID_, const map<int, string > &vocab) {
	auto product_name = vocab.find(ID_);
	if (product_name == vocab.end())
		throw(exception("No product with this ID"));
	return product_name->second;
}

class shop_chain {
private:
	map<int, shop> shop_list;
	map<int, string> available_products;
	int ID = 0;
	int shop_ID = 0;
public:
	shop_chain() = default;
	//task 1 создать магазины
	void add_shop(string shop_name_, string shop_adress_) {
		shop_ID++;
		shop shop_(shop_ID, shop_name_, shop_adress_);
		auto res = shop_list.emplace(shop_ID, shop_);
	}
	//task2 добавить продукт
	void add_product(string product_name_) {
		ID++;
		available_products.emplace(ID ,product_name_);
		for (auto cur = shop_list.begin(); cur != shop_list.end(); cur++) {
			cur->second.add_new_product(product_name_, 0, 0);
		}
	}
	//task3 завести партию товаров в магазины
	void deliver_products_in_shop(int shop_ID_, const map<int,pair<int,int>> &products_) {
		auto shop = shop_list.find(shop_ID_);
		for (auto cur = products_.begin(); cur != products_.end(); cur++) {
			string product_name = find_product_name(cur->first, available_products);
			shop->second.deliver_new_product_(product_name, cur->second.first, cur->second.second);
		}
		
	}
	//task4 найти самый дешевый товар в магазинах
	vector<string> find_cheapest_product(int product_ID_) {
		string product_name = find_product_name(product_ID_, available_products);
		multimap<int, string> res;
		vector<string> result;
		for (auto cur = shop_list.begin(); cur != shop_list.end(); cur++) {
			int price = cur->second.get_product_price(product_name);
			string shop_name = cur->second.get_shop_name();
			if (price != max_int){
				res.emplace(price, shop_name);
			}
		}
		if (!res.empty()) {
			auto cur = res.begin();
			while (cur != res.end()) {
				if (cur->first == res.begin()->first)
					result.push_back(cur->second);
				cur++;
			}
		}
		else
			throw (exception("There is no such product in any of the shops"));
		return result;
	}
	//task5 понять, ĸаĸие товары можно ĸупить в магазине на неĸоторую сумму
	map<string, int> buy_products(int shop_ID_,int price_) {
		map<string, int> result;
		auto shop = shop_list.find(shop_ID_);
		check_shop_existance(shop_ID_, shop_list);
		for (auto cur = available_products.begin(); cur != available_products.end(); cur++) {
			int product_price_ = shop->second.get_product_price(cur->second);
				if (product_price_ <= price_ && product_price_ != 10e8) {
					result.emplace(cur->second, price_ / product_price_);
				}
		}
		return result;
	}
	//task6 купить партию товаров в магазине
	string make_purchase(int shop_ID_, const map<int, int> &product_ID_amount_) {
		int purchuase_price = 0;
		auto shop = shop_list.find(shop_ID_);
		check_shop_existance(shop_ID_, shop_list);
		for (auto cur = product_ID_amount_.begin(); cur != product_ID_amount_.end(); cur++) {
			string product_name = find_product_name(cur->first, available_products);
			int product_price = shop->second.get_product_price(product_name);
			int product_amount = shop->second.get_product_amount(product_name);
			if (product_amount >= cur->second) {
					purchuase_price += product_price * cur->second;
			}
			else {
				return "Not enough products in shop";
			}
		}
			return to_string(purchuase_price);
	}
	//task7 Найти, в ĸаĸом магазине партия товаров (набор товар-ĸоличество) имеет наименьшую сумму
	vector<string> find_cheapest_purchase(const map<int, int>& product_name_amount_) {
		multimap<int, string> res;
		vector<string> result;
		for (auto cur = shop_list.begin(); cur != shop_list.end(); cur++) {
			string result = make_purchase(cur->first, product_name_amount_);
			if (result != "Not enough products in shop") {
				res.emplace(stof(result), cur->second.get_shop_name());
			}
		}
		if (!res.empty()) {
			auto cur = res.begin();
			while (cur != res.end()) {
				if (cur->first == res.begin()->first)
					result.push_back(cur->second);
				cur++;
			}
		}
		else
			throw (exception("Purchase can't be made in any of the shops"));
		return result;
	}
	int get_product_ID(string product_name_) {
		for (auto cur = available_products.begin(); cur != available_products.end(); cur++)	{
			if (cur->second == product_name_)
				return cur->first;
		}
	}
	int get_shop_ID(string shop_name_) {
		for (auto cur = shop_list.begin(); cur != shop_list.end(); cur++) {
			if (cur->second.get_shop_name() == shop_name_)
				return cur->first;
		}
	}
	void show() {
		for ( auto cur = shop_list.begin(); cur != shop_list.end(); cur++)
		{
			cur->second.show_products();
		}
	}
};
void fill_shops(shop_chain &shops) {
	try {
		map<int, pair<int, int>> friday_delivery;
		friday_delivery.emplace(9, make_pair(50, 399));
		friday_delivery.emplace(8, make_pair(10000, 50));
		friday_delivery.emplace(2, make_pair(150, 99));
		friday_delivery.emplace(5, make_pair(123, 199));
		friday_delivery.emplace(1, make_pair(1599, 100));
		friday_delivery.emplace(7, make_pair(199, 200));
		friday_delivery.emplace(6, make_pair(259, 65));
		friday_delivery.emplace(4, make_pair(19, 1299));
		friday_delivery.emplace(3, make_pair(120, 599));
		shops.deliver_products_in_shop(1, friday_delivery);
		map<int, pair<int, int>> sunday_delivery;
		sunday_delivery.emplace(3, make_pair(1010, 500));
		sunday_delivery.emplace(8, make_pair(10000, 25));
		sunday_delivery.emplace(6, make_pair(3000, 75));
		sunday_delivery.emplace(9, make_pair(50, 1290));
		sunday_delivery.emplace(1, make_pair(50000, 120));
		sunday_delivery.emplace(5, make_pair(300, 75));
		sunday_delivery.emplace(4, make_pair(10, 1500));
		sunday_delivery.emplace(2, make_pair(1000, 150));
		shops.deliver_products_in_shop(2, sunday_delivery);
		map<int, pair<int, int>> wednesday_delivery;
		wednesday_delivery.emplace(9, make_pair(12, 990));
		wednesday_delivery.emplace(8, make_pair(10000,35));
		wednesday_delivery.emplace(2, make_pair(3000, 105));
		wednesday_delivery.emplace(5, make_pair(340, 100));
		wednesday_delivery.emplace(1, make_pair(500, 200));
		wednesday_delivery.emplace(7, make_pair(500, 88));
		wednesday_delivery.emplace(6, make_pair(799, 69));
		wednesday_delivery.emplace(4, make_pair(5, 2000));
		wednesday_delivery.emplace(3, make_pair(399, 990));
		shops.deliver_products_in_shop(3, wednesday_delivery);
	}
	catch (exception & ex) {
		cout << ex.what();
	}
}

int main() {
	shop_chain Ashan_SPB;
	try {
		//task 1 создать магазины
		Ashan_SPB.add_shop("Ashan_Obvodniy_Canal", "Borovaya,47");
		Ashan_SPB.add_shop("Ashan_Staraya_Derevnya", "Torfiannaya,7");
		Ashan_SPB.add_shop("Ashan_Murino", "Prospect_Kulturi,4");
		//task2 добавить продукты
		Ashan_SPB.add_product("orange");
		Ashan_SPB.add_product("apple");
		Ashan_SPB.add_product("fish");
		Ashan_SPB.add_product("crab");
		Ashan_SPB.add_product("snickers");
		Ashan_SPB.add_product("milk");
		Ashan_SPB.add_product("sausages");
		Ashan_SPB.add_product("bread");
		Ashan_SPB.add_product("pan");
		Ashan_SPB.add_product("multicooker");
		//task3 завести партию товаров в магазины
		fill_shops(Ashan_SPB);
		Ashan_SPB.show();
		//task4 найти самый дешевый товар в магазинах
		vector<string> cheapest_pan =  Ashan_SPB.find_cheapest_product(9);
		cout << "task_4=========------\n"<<"Cheapest product is in: " << "\n";
		for (size_t i = 0; i < cheapest_pan.size(); i++)
		{
			cout << cheapest_pan[i] << "\n";
		}
		//task5 понять, ĸаĸие товары можно ĸупить в магазине на неĸоторую сумму
		cout << "task_5=========-------\n";
		map<string, int> check;
		check = Ashan_SPB.buy_products(3, 500);
		cout << "You can by"  << " for: " << 500 << "RUB\n";
		for (auto cur = check.begin(); cur != check.end(); cur++) {
			cout << cur->first << " x " << cur->second << "\n";
		}
		//task6 купить партию товаров в магазине
		map<int, int> purchuase_Mihail;
		purchuase_Mihail.emplace(8, 2);
		purchuase_Mihail.emplace(6, 1);
		purchuase_Mihail.emplace(1, 1);
		cout << "task_6=========--------\n" <<"Mihail's purchase cost: "
			<<Ashan_SPB.make_purchase(3, purchuase_Mihail) << "\n";
		//task7 Найти, в ĸаĸом магазине партия товаров (набор товар-ĸоличество) имеет наименьшую сумму 
		vector<string> cheapest_purchuase_for_Mihail = Ashan_SPB.find_cheapest_purchase(purchuase_Mihail);
		cout << "task_7=========--------\n" <<"The cheapest purchase will be in: \n";
		for (size_t i = 0; i < cheapest_purchuase_for_Mihail.size(); i++)
		{
			cout << cheapest_purchuase_for_Mihail[i] << "\n";
		}
	}
	catch(exception &ex) {
		cout << ex.what();
	}
}
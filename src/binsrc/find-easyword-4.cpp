#include "../cppcore/core.impl.cpp"

//查找输出easyword-4

//easyword-4: 在 abcd 中 ab或cd在词库中已经存在被认为是easyword-4
//e.g. '你好世界' 中 '你好'和'世界' 在词库中存在, easyword-4

using namespace table;

//                             0    1   first 2
//target           被预期fmt: null,null=字词,null
static bool not_easyword_4_filter(const pair<string,vector<string>> &p,table_t &table){
	if (utf8_length(p.first) != 4)return false;

	size_t e1 = utf8_word_locate(p.first,0);
	size_t e2 = utf8_word_locate(p.first,e1);
	size_t e3 = utf8_word_locate(p.first,e2);

	//第一个部分 abcd 中的 ab
	string_view p1 = string_slice(p.first,0,e2);

	//第二个部分 abcd 中的 cd
	string_view p2 = string_slice(p.first,e2);

	auto find_p1 = table.find(string(p1));
	auto find_p2 = table.find(string(p2));

	if (find_p1 != table.end() && find_p2 != table.end()){
		return true;
	}

	return false;
}

int main(int argc ,char *argv[]) try {
	if (argc != 2){
		cerr << format("Too many or too few argument.") << endl;
		cerr << format("Usage: {} 集合.utf8txt ",argv[0]) << endl;
		throw std::runtime_error("Error arguments.");
	}

	error_type error = nullptr;
	bool is_ok = false;

	table::table_t a(argv[1],table_category::key_word);
	if (!a.is_ok){throw std::runtime_error(a.error);}

	epcall(a.output_table(cout,
		[&](const pair<string,vector<string>> p)->bool{
			return not_easyword_4_filter(p,a);
		}),
	error,is_ok);

	if(!is_ok)cerr << error << endl;

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}





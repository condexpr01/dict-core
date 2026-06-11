#include "../cppcore/core.impl.cpp"

using namespace table;

class u8string_index{
public:
	vector<size_t> index{};
	size_t end{};//[0,end)

	size_t operator[](size_t pos){
		return pos < end ? index[pos] : string::npos;
	}

	u8string_index(const string &s){
		for(size_t pos{0}; pos < s.size() ;){
			index.push_back(pos);
			pos = table::utf8_word_locate(s,pos);
		}

		end = index.size();
	}

	u8string_index(string &&s) = delete;
	u8string_index& operator=(u8string_index &)  = delete;
	u8string_index& operator=(u8string_index &&) = delete;

	~u8string_index() = default;

};

string hanzi_to_pinyin(const string &hanzi,table_t &pinyin){
	string result{};

	u8string_index u8i{hanzi};

	size_t pos{0};

	table_t::key_list kl;
	std::pair<string,vector<string>> max;

	for(size_t win{};u8i[pos] != string::npos;pos+=win){

		//长度最长是4字成语
		win = 4;

		//使win的长度有效
		for(;u8i[pos+win-1]==string::npos;win--);

		//查找hanzi的[pos,pos+win)
		while(win>0){
			kl = pinyin.find_list(hanzi.substr(u8i[pos],u8i[pos+win] - u8i[pos]));

			if(kl.begin() != kl.end())break;//命中
			else win--;//缩小区间
		}

		if (win > 0){

			//选频数最高的一个
			max = *kl.begin();
			for(auto &&e : kl){
				if (max.second[2].length() <  e.second[2].length()
				|| (max.second[2].length() == e.second[2].length() && max.second[2] < e.second[2]) ){
					max = e;
				}
			}

			result += '_' + max.second[0];

		}else{//missing
			result += "_*";
			win=1;//jump and for check err

			cerr << "[hanzi_to_pinyin]miss: " << hanzi.substr(u8i[pos],u8i[pos+win] - u8i[pos]) << endl;
		}

	}

	result.erase(0,1);//去掉开头的下划线
	return result;
}

int main(int argc ,char *argv[]) try {

	error_type error = nullptr;
	bool is_ok = false;

	if (argc != 3){
		cerr << format("Too many or too few argument.") << endl;
		cerr << format("Usage: {} 拼音表.utf8txt 待编码表.utf8txt",argv[0]) << endl;
		throw std::runtime_error("Error arguments.");
	}

	//                                        0    1   first 2
	//cat为key_word ,期望键存在字词,对应位置:编码,序号=[字词],频数

	table_t pinyin{argv[1],table_category::key_word};
	if(!pinyin.is_ok){throw std::runtime_error{pinyin.error};}

	table_t to{argv[2],table_category::key_word};
	if(!to.is_ok){throw std::runtime_error{to.error};}
	
	for (auto&& k : to){
		k.second[0] = hanzi_to_pinyin(k.first,pinyin);
	}

	epcall(to.output_table(cout),error,is_ok);
	if(!is_ok)cerr << error << endl;

	return 0;

} catch (std::exception &e){
	cerr << e.what() << endl;
	return 1;

} catch (int v){
	return v;
}





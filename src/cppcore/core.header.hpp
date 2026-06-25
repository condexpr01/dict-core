#ifndef DICT_CORE_HEADER_GUARD
#define DICT_CORE_HEADER_GUARD

#include <iostream>
#include <fstream>

#include <concepts>

#include <expected>
#include <filesystem>

#include <unordered_map>
#include <vector>
#include <array>

#include <string>
#include <cstring>

#include <string_view>

namespace table{

	//错误串类型
	typedef const char* error_type;

	//错误类型
	template<typename T>
		using ep = std::expected<T,error_type>;

	using unep = std::unexpected<error_type>;

	//错误串空检测
	template<typename errT>
		bool is_error_empty(errT error)
		requires requires(errT s){
			{!s}->std::convertible_to<bool>;
			{*s}->std::convertible_to<const char>;
		}{
			return !error || (*error == '\0');
		}

	//便捷调用并转移值
	//当然也可以用expected原生and_then, or_else, transform, transform_error
	template <typename epT,typename returnT>
		void epcall(epT&& epr, returnT &value, error_type &error, bool &is_ok) {
			if (epr.has_value()) {
				is_ok = true;
				error = nullptr;
				value = std::move(epr.value());
			} else {
				is_ok = false;
				error = epr.error(); // 指针无需move
			}
		}

	//epcall弃值版重载
	template <typename epT>
		void epcall(epT&& epr, error_type &error, bool &is_ok) {
			if (epr.has_value()) {
				is_ok = true;
				error = nullptr;
			} else {
				is_ok = false;
				error = epr.error();
			}
		}

	//epcall弃值,弃状态版重载
	template <typename epT>
		void epcall(epT&& epr, error_type &error) {
			if (epr.has_value()) {
				error = nullptr;
			}else{
				error = epr.error();
			}
		}


	//ios
	using std::cout;
	using std::cin;
	using std::clog;
	using std::cerr;
	using std::endl;

	//fsys
	using std::filesystem::directory_iterator;

	//流
	using std::ifstream;
	using std::ofstream;
	using std::istream;
	using std::ostream;
	using std::format;

	//串
	using std::getline;
	using std::string;
	using std::string_view;

	//容器
	using std::unordered_multimap;
	using std::vector;
	using std::array;
	using std::pair;
	using std::get;

	//concepts
	using std::same_as;
	using std::remove_cvref_t;
	using std::is_const_v;


	//表flag
	//cat为key_codec,期望键存在编码,对应位置:[编码],序号=字词,频数
	//cat为key_word ,期望键存在字词,对应位置:编码,序号=[字词],频数
	enum class table_category: int{
		key_none,  //无键状态
		key_codec, //编码为键哈希表的flag
		key_word,  //字词为键哈希表的flag
	};

	//表对象类
	class table_t{
		public:
			//预期格式：编码,序号=字词,频数
			using table_type = unordered_multimap<string,vector<string>>;

			//表对象的哈希表
			table_type table;

			//表对象的flag
			table_category category = table_category::key_none;

			//构造表对象时的错误信息
			error_type error = nullptr;
			bool is_ok = false;

			using iterator = table_type::iterator;
			using const_iterator = table_type::const_iterator;

			//用单字表编码当前表
			ep<void> encoder(const table_t &single_word_table);

			//从from表获取频数到当前表
			ep<void> get_freq(table_t &from);

			//字集 交集，并集，差集
			ep<void> word_set_intersect(const table_t &with);
			ep<void> word_set_unite(const table_t &with);
			ep<void> word_set_difference(const table_t &with);

			//编码 交集，并集，差集
			ep<void> codec_set_intersect(const table_t &with);
			ep<void> codec_set_unite(const table_t &with);
			ep<void> codec_set_difference(const table_t &with);

			//字和编码 交集，并集，差集
			ep<void> word_codec_set_intersect(const table_t &with);
			ep<void> word_codec_set_unite(const table_t &with);
			ep<void> word_codec_set_difference(const table_t &with);

			//默认输出不过滤器(返回true:输出这项，返回false不输出这项)
			static bool default_filter(const pair<string,vector<string>> &p){return true;}

			//在编码后，要求不输出编码失败的项
			static bool encoder_filter(const pair<string,vector<string>> &p){
				if (p.second[0].empty())return false;
				return true;}

			//在获频后，要求不输出无频的项
			static bool get_freq_filter(const pair<string,vector<string>> &p){
				if (p.second[2].empty())return false;
				return true;}


			//输出表对象到输出流
			template <typename T = bool(*)(const pair<string,vector<string>>&)>
				requires requires(T f,pair<string,vector<string>> p){{f(p)}->same_as<bool>;}
			ep<void> output_table(ostream &out, T filter = table_t::default_filter);

			//从输入流构造表对象, cat为表flag(返回必正常,无unep)
			ep<void> make_table(ifstream& ifs, table_category cat = table_category::key_codec);

			//从文件构造表对象, cat为表flag
			table_t(const string &filename, table_category cat = table_category::key_codec){

				ifstream ifs{filename};

				if (ifs.is_open()){
					epcall((*this).make_table(ifs,cat), error, is_ok);
				}else{
					error =  "[table_t method]Failed to open the file.";
				}
			}

			//从ep<ifstream>构造表对象, cat为表flag
			template<typename EP>
				requires same_as<remove_cvref_t<EP>, ep<ifstream>>
				table_t(EP &&ep_ifs, table_category cat = table_category::key_codec){
					ifstream ifs;
					epcall(std::move(ep_ifs),ifs,error,is_ok);

					if (is_ok){epcall((*this).make_table(ifs,cat),error,is_ok);}
				}

			//相同的key的list上迭代, 用给range for
			struct key_list{
				//iterator.first  string
				//iterator.second vector<string>
				pair<iterator,iterator> range;//从equal_range构造

				iterator begin() noexcept{return range.first;}
				iterator end()   noexcept{return range.second;}
				const_iterator begin()const noexcept{return range.first;}
				const_iterator end()  const noexcept{return range.second;}
			};

			struct key_list_const{
				//iterator.first  string
				//iterator.second vector<string>
				pair<const_iterator,const_iterator> range;//从equal_range构造

				const_iterator begin()const noexcept{return range.first;}
				const_iterator end()  const noexcept{return range.second;}
			};

			key_list_const find_list(const string &key) const
			{return key_list_const{table.equal_range(key)};}

			key_list find_list(const string &key)
			{return key_list{table.equal_range(key)};}


			~table_t() = default;
			table_t() = default;

			void operator=(table_t &t){
				(*this).table = t.table;
				(*this).category = t.category;
			}

			void operator=(table_t &&t){
				(*this).table = std::move(t.table);
				(*this).category = t.category;
			}

			table_t(table_t &t){
				(*this).table = t.table;
				(*this).category = t.category;
			}

			table_t(table_t &&t){
				(*this).table = std::move(t.table);
				(*this).category = t.category;
			}

			iterator begin() noexcept{return table.begin();}
			iterator end()   noexcept{return table.end();}

			const_iterator begin() const noexcept{return table.begin();}
			const_iterator end()   const noexcept{return table.end();}

			const_iterator cbegin() const noexcept{return table.cbegin();}
			const_iterator cend()   const noexcept{return table.cend();}

			pair<iterator,iterator>             equal_range(const string &key)     {return table.equal_range(key);}
			pair<const_iterator,const_iterator> equal_range(const string &key)const{return table.equal_range(key);}

			iterator find(const string &key){return table.find(key);}
			const_iterator find(const string &key) const{return table.find(key);}

			iterator erase(iterator i){return table.erase(i);}
			iterator erase(const_iterator i) {return table.erase(i);}

			size_t size() const {return table.size();};
	};

	//从命令行参数中检测文件
	ep<ifstream> detect_file_from_args(const int& argc ,const char * const * const &argv);

	//递归地显示目录结构
	ep<void> dir_layout(ostream& output,const string &s_path);

	//用vector<pair<string,vector<string>>>表示的table,
	//与table_t不同的是, 元素顺序与读入先后顺序有关
	ep<void> make_vector_table(ifstream& ifs,vector<pair<string,vector<string>>>& v,const table_category cat);

	//string切片视图,闭开区间(返回必正常,无unep)
	string_view string_slice(const string_view &s,const size_t startpos,const size_t endpos = string::npos) noexcept;

	//将sep_set中的字符作为分隔符，切片s到vector中
	ep<void> string_sep_vector(const string & s, const string & sep_set, vector<string> &v);

	//字符串加法
	ep<std::string> string_add(const std::string& num1, const std::string& num2);

	//字符串减法(负数会变为0, 因为不希望出现负频数)
	ep<std::string> string_sub(const std::string& num1, const std::string& num2);

	//u8长度(返回必正常,无unep)
	size_t utf8_length(const string &s) noexcept;

	//u8从begin开始的结束位置定位,闭开[begin,end)(返回必正常,无unep)
	size_t utf8_word_locate(const string &s, size_t begin) noexcept;

	//过滤不可见字符(返回必正常,无unep)
	string string_visiable(const string &s);

	//编码器(从single_word_table编码target)
	ep<void> encoder(const table_t &single_word_table, table_t &target);

	//从from取频数到to
	ep<void> get_freq(const table_t &from, table_t &to);
	ep<void> get_freq(const table_t &from, vector<pair<string,vector<string>>> &to);

	ep<void> word_set_intersect(table_t &intersection,const table_t &with);
	ep<void> word_set_unite(table_t &union_set,const table_t &with);
	ep<void> word_set_difference(table_t &difference,const table_t &with);

	ep<void> word_codec_set_intersect(table_t &intersection,const table_t &with);
	ep<void> word_codec_set_unite(table_t &union_set,const table_t &with);
	ep<void> word_codec_set_difference(table_t &difference,const table_t &with);

	ep<void> codec_set_unite(table_t &union_set,const table_t &with);
	ep<void> codec_set_intersect(table_t &intersection,const table_t &with);
	ep<void> codec_set_difference(table_t &difference,const table_t &with);

	template <typename T>
		requires requires(T f,pair<string,vector<string>> p){{f(p)}->same_as<bool>;}
	ep<void> table_t::output_table(ostream &out, T filter){

		if((*this).category == table_category::key_codec){
			for (auto &&v : (*this).table){
				if(!filter(v))continue;

				//格式: 编码[键值],序号0=字词1,频数2
				out << format("{},{}={},{}\n",
						string_visiable(v.first),
						string_visiable(v.second[0]),
						string_visiable(v.second[1]),
						string_visiable(v.second[2])
						);

				out.flush();
			}}

		if((*this).category == table_category::key_word){
			for (auto &&v : (*this).table){
				if(!filter(v))continue;

				//格式: 编码0,序号1=字词[键值],频数2
				out << format("{},{}={},{}\n",
						string_visiable(v.second[0]),
						string_visiable(v.second[1]),
						string_visiable(v.first),
						string_visiable(v.second[2])
						);

				out.flush();
			}}

		return {};
	}

}
#endif

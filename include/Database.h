#ifndef DATABASE_H
#define DATABASE_H

namespace hiberlite{

class ModelExtractor;
class UpdateBean;

class Database : noncopyable
{
	private:
		friend class UpdateBean;
		friend class KillChildren;
		friend class LoadBean;

		template<class C>
		friend class real_bean;

		ModelExtractor* mx;
		bool transaction_in_progress;

		template<class C>
		static void dbDelete(bean_key key, C& bean);

		template<class C>
		static void dbUpdate(bean_key key, C& bean);

		template<class C>
		static C* dbLoad(bean_key key);

		static sqlid_t allocId(shared_connection c, std::string table);
		sqlid_t allocId(std::string table);

		static void dbExecQuery(shared_connection con, std::string query);

		static std::vector<sqlid_t> dbSelectIds(shared_connection con, const std::string table,
														const std::string condition, const std::string orderBy);

		static std::vector<sqlid_t> dbSelectChildIds(shared_connection con, std::string table, sqlid_t parent);
		inline std::vector<sqlid_t> dbSelectChildIds(std::string table, sqlid_t parent);

		shared_connection con;

		template<class C>
		static void dbDeleteRows( shared_connection con, std::string table, std::string column, C value );

	public:
		static const sqlid_t NULL_ID=-1;

		template<class C>
		static std::string getClassName();

		Database();
		Database(std::string fname);
		virtual ~Database();

		void open(std::string fname);
		void close();

		template<class C>
		inline void registerBeanClass();
		
		void begin_transaction();
		void commit_transaction();

		Model getModel();
		inline shared_connection getConnection() { return con; }

		std::vector<std::string> checkModel();
		void dropModel();
		void createModel();

		template<class C>
		bean_ptr<C> loadBean(sqlid_t objId);

		template<class C>
		std::vector<sqlid_t> getBeanIds();
		
		template<class C>
		//std::vector< bean_ptr<C> > getAllBeanAfter(const sqlid_t id);
		std::vector<sqlid_t> getBeanIds(const std::string condition="", const std::string orderBy="");

		template<class C>
		std::vector< bean_ptr<C> > getAllBeanAfter(const sqlid_t id);
		
		template<class C>
		//std::vector< bean_ptr<C> > getAllBeans();
		std::vector< bean_ptr<C> > getAllBeans(const std::string condition="", const std::string orderBy="");

		template<class C>
		bean_ptr<C> copyBean(const C& c);

		template<class C>
		bean_ptr<C> createBean();

		template<class C>
		bean_ptr<C> manageBean(C* ptr);
		
		void dbExecQuery(std::string query);
};

} //namespace hiberlite;


#endif // DATABASE_H

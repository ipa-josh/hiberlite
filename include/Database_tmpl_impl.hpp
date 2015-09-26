#ifndef DATABASE_TMPL_IMPL_HPP_INCLUDED
#define DATABASE_TMPL_IMPL_HPP_INCLUDED

namespace hiberlite{

template<class C>
void Database::dbDelete(bean_key key, C& bean)
{
	if(!key.transaction_in_progress)  {
		try{
			dbExecQuery(key.con,"ROLLBACK TRANSACTION;");
		}catch(...){}
		dbExecQuery(key.con,"BEGIN TRANSACTION;");
	}

		ChildKiller ck;
		ck.killChildren(key,bean);
		dbDeleteRows(key.con, Database::getClassName<C>(), HIBERLITE_PRIMARY_KEY_COLUMN, key.id);

	if(!key.transaction_in_progress) dbExecQuery(key.con,"COMMIT TRANSACTION;");
}

template<class C>
void Database::dbDeleteRows(shared_connection con, std::string table, std::string column, C value )
{
	std::string query="DELETE FROM "+table
					+ " WHERE "+column+"="+Transformer::toSQLiteValue(value)+";";
	dbExecQuery(con,query);
}


template<class C>
void Database::dbUpdate(bean_key key, C& bean)
{
	if(!key.transaction_in_progress) {
		try{
			dbExecQuery(key.con,"ROLLBACK TRANSACTION;");
		}catch(...){}
		dbExecQuery(key.con,"BEGIN TRANSACTION;");
	}

		ChildKiller ck;
		ck.killChildren(key,bean);
		BeanUpdater u;
		u.update(key, bean);

	if(!key.transaction_in_progress) dbExecQuery(key.con,"COMMIT TRANSACTION;");
}

template<class C>
C* Database::dbLoad(bean_key key)
{
	BeanLoader bl;
	return bl.loadBean<C>(key);
}

template<class C>
inline bean_ptr<C> Database::createBean()
{
	C* obj=new C();
	return manageBean<C>(obj);
}

template<class C>
bean_ptr<C> Database::copyBean(const C& c)
{
	return manageBean<C>( new C(c) );
}

template<class C>
inline bean_ptr<C> Database::manageBean(C* ptr)
{
	sqlid_t id=allocId( Database::getClassName<C>() );
	bean_key key(con,id,transaction_in_progress);
	dbUpdate(key,*ptr);
	return Registry<C>::createBeanPtr(key,ptr);
}

template<class C>
inline bean_ptr<C> Database::loadBean(sqlid_t id)
{
	return Registry<C>::get( bean_key(con,id,transaction_in_progress) );
}

template<class C>
std::vector<sqlid_t> Database::getBeanIds(const std::string condition, const std::string orderBy)
{
	return dbSelectIds(con, getClassName<C>(), condition, orderBy);
}

template<class C>
std::vector< bean_ptr<C> > Database::getAllBeans(const std::string condition, const std::string orderBy)
{
	std::vector<sqlid_t> ids=getBeanIds<C>(condition, orderBy);
	size_t N=ids.size();
	std::vector< bean_ptr<C> > ans;
	ans.reserve(N);
	for(size_t i=0;i<N;i++)
		ans.push_back( loadBean<C>(ids[i]) );
	return ans;
}

template<class C>
std::vector< bean_ptr<C> > Database::getAllBeanAfter(const sqlid_t id)
{
	std::vector<sqlid_t> ids=dbSelectIds(con, getClassName<C>(),std::string(HIBERLITE_PRIMARY_KEY_COLUMN)+">"+Transformer::toSQLiteValue(id), "" );
	size_t N=ids.size();
	std::vector< bean_ptr<C> > ans;
	ans.reserve(N);
	for(size_t i=0;i<N;i++)
		ans.push_back( loadBean<C>(ids[i]) );
	return ans;
}


template<class C>
inline void Database::registerBeanClass()
{
	if(!mx)
		mx=new ModelExtractor();
	std::string classname=Database::getClassName<C>();

	mx->extractModel<C>(con);
}

} //namespace hiberlite

#endif // DATABASE_TMPL_IMPL_HPP_INCLUDED

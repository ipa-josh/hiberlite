#ifndef BEAN_PTR_H_INCLUDED
#define BEAN_PTR_H_INCLUDED

#include <boost/shared_ptr.hpp>


namespace hiberlite{

class Database;
class access;

template<class C>
class Registry;

template<class C>
class shared_res;

template<class C>
class real_bean : noncopyable {
	public:
		inline C* operator->();
		inline C* get();

		~real_bean();

		bean_key get_key() const { return key; }

		void save();

		void destroy();
		bool destroyed() const { return forgotten; }
		void set_only_ref() {only_ref=true;}

	protected:
		inline void loadLazy();

		bean_key key;
		C* obj;
		bool forgotten, only_ref;

	private:
		friend class Registry<C>;
		real_bean(const bean_key _key, C* _obj); //only Registry can create the real_bean
};

template<class C>
class bean_ptr : public shared_res< real_bean<C> >
{
	friend class hiberlite::access;
	template<class Archive>
	void hibernate(Archive & ar);

	friend class Registry<C>;
	bean_ptr(bean_key k, rb_pair<C>* rbpair);

	sqlid_t tmp_id;

	public:
		bean_ptr(bean_key k);

		bean_ptr();

		operator bool() const;

		bean_ptr(const bean_ptr<C>& other);
		bean_ptr<C>& operator=(const bean_ptr<C>& other);

		void save();
		void destroy();

		bool destroyed() {
			return shared_res< real_bean<C> >::get_object()->destroyed();
		}
		boost::shared_ptr<C> shared_ptr() {
			C *ptr = shared_res< real_bean<C> >::get_object()->get();
			shared_res< real_bean<C> >::get_object()->set_only_ref();
			return boost::shared_ptr<C>( ptr );
		}

		inline sqlid_t get_id() const;
		inline C& operator*();
		inline C* operator->();
};

}//namespace hiberlite

#endif // BEAN_PTR_H_INCLUDED

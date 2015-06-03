#ifndef __DICTIONARY_H__
#define	__DICTIONARY_H__
#include <cstring>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <exception>
#include <cstdio>
#include "throw.h++"
#include "commons.h++"
namespace Scr
{
	
	/*!
	 * \brief replacement of std::map<std::string,T> - optimized for string key random access
	 * using dictionary-tree data structure.
	 *
	 * Member functions are named in C++ library convention, that is
	 * w/ underscore and w/o capital letters. 
	 * \note this class is not STL compatible. it is only STL-like. 
	 */
	template<typename T>
	class Dictionary
	{
	protected:
		static const int T_VECTOR = 2;
		static const int T_RECORD = 4;
		static const size_t num_characters = 256;
		
		struct t_name_vector;
		//! tree leaf (node containing just one pc. of information
		struct t_name_record
		{
			int type;//!< magic value to test, whenever it is a vector or a record
			t_name_vector * parent;
			char * name;//!< key itself
			int num_occurrences;//!< number of occurences of specific key
			T * datafield;//! datafield
		} ;
		
		//! node containing references to other nodes
		struct t_name_vector
		{
			int type;//!< magic value to test, whenever it is a vector or a record
			t_name_vector * parent;
			t_name_record * vector[num_characters];/*'\0' and 'A' to 'Z'*/
		} ;
		
		//! core information block (one per Dictionary)
		typedef struct
		{
			int max_num_occurrences;//!< greatest recorded number of occurrences
			t_name_vector * first_vector;//!< first vector
		} t_tree;
		
	private:
		t_tree tree; /*tree-like structure*/
	protected:
#define vmark(x) (x)
		
#define record vector[ vmark( static_cast<size_t>(name[current]) ) ]
		/*!
		 * add node to tree.
		 *
		 * \param name name associated w/ node
		 * \return pointer to new node (or NULL if adding it was unsuccessful)
		 * \exception std::bad_alloc if memory allocation failed
		 */
		t_name_record * tree_add (const char * name);
		
		/*!
		 * Attempts to search for a specific node. Doesn't modify tree (doesn't
		 * new node if search failed).
		 *
		 * \return Tf argument matches beginning of more than
		 * node key, t_name_vector is really returned (what may be detected
		 * by testing type member field), even if one of theese nodes matches
		 * completely. If nothing matches, 0 is returned. Otherwise ptr to record is
		 * returned
		 * 
		 * \param name key to look for
		 * \param current_vector where to start search
		 * \param current assume current depth in tree (start matching from this
		 *  character of name)
		 */
		t_name_record * tree_partial_find (const char * name,
				t_name_vector * current_vector,  size_t current = 0) const;
		
		/*!
		 * \return pointer to specific node if it exists, NULL otherwise. this 
		 * function depends on tree_partial_search
		 *
		 * \param name key to look for
		 * \param current_vector where to look for
		 * \param current assume current depth in tree (start matching from this
		 *  character of name)
		 */
		t_name_record * tree_find (const char * name,
				t_name_vector * current_vector, int current = 0) const;
		
		/*!
		 * Find next node. If r points to vector, find it's first node.
		 * If nothing found, return 0.
		 *
		 * \param r record
		 */
		static t_name_record * tree_find_next (t_name_record * r);

		
		/*!
		 * erase record
		 *
		 * \param r record to be erased
		 */
		static void tree_erase_record (t_name_record * r)
		{
			delete r->datafield;
			free (r->name);
			free (r);
		}
		
		/*!
		 * erase vector
		 *
		 * \param v pointer to vector, that will be erased
		 *
		 * \note function not only recursively erases contents of vector, but
		 *  also erases vector itself
		 */
		static void tree_erase_vector (t_name_vector * v);
	public:
		
		//! \a iterator class for Dictionary
		class iterator
		{
		private:
			t_name_record * _node;
			
		public:
			/*!
			 * Default constructor returns iterator, that equals end()
			 */
			iterator ()
			{
				_node = 0; // 0 is special code for end, as no node of this address exists
			}
			
			/*!
			 * copy constructor
			 *
			 * \param it base of construction
			 */
			iterator (const iterator & it)
			{
				_node = it._node;
			}
			
		protected:
			
			/*!
			 * Constructor initialized w/ raw data node pointer (t_name_record)
			 * is accessed by functions such as begin(), end() or find().
			 *
			 * \param __node node in tree mapped to this iterator
			 */
			explicit iterator (t_name_record * __node)
			{
				_node = __node;
			}
			
		public:
			
			//! result of validity_test
			enum validity
			{	//! dereference (indirection) possible, iterator points to
				//!   single data object
				VALID,
				//! unique key, but no data object (dereference WILL fail)
				INVALID,
				//! not unique key: dreference WILL fail
				NOT_UNIQUE,
				//! end(): dreference WILL fail
				END
			};
			
			/*!
			 * Tests if iterator is valid. If it is VALID is returned. if it is not,
			 * function says why
			 */
			validity validity_test ()
			{
				if (_node not_eq NULL)
				{
					if (_node->type==T_RECORD)
					{
						if	(_node->datafield)
							return VALID;
						else
							return INVALID;
					}
					else
						return NOT_UNIQUE;
				}
				else
					return END;
			}//iterator::validity_test
			
			/*!
			 * tests if iterator is valid
			 */
			bool valid ()
			{
				return validity_test ()==VALID;
			}//iterator::valid
			
			/*!
			 * Indirection operator returns reference to object
			 * \throw std::bad_exception happens when iterator is not unique
			 */
			T& operator* ()
			{
				using namespace std;
				RexIOLog(LogLevelModerate) << "Dereferencing " << _node->name << endl;
				switch(validity_test ())
				{
					case INVALID:
						_node->datafield = new T (); // make it valid, and
						// process it as if it was valid
					case VALID:
						return  * _node->datafield;
						
					default:;
				}
				throw std::bad_exception ();
				
			}// iterator::operator*
			
			/*!
			 * Indirection-and-element-access operator returns reference to object
			 * \throw std::bad_exception happens when iterator is not unique
			 */
			T* operator -> ()
			{
				switch(validity_test ())
				{
					case INVALID:
						_node->datafield = new T (); // make it valid, and
						// process it as if it was valid
					case VALID:
						return  * _node->datafield;
					case NOT_UNIQUE:
						THROW (LogicError);
				}
			}// iterator::operator ->
			
			/*!
			 * Assignment operator
			 *
			 * \param it other iterator
			 */
			iterator & operator=(const iterator & it)
			{
				_node = it._node;
				return *this;
			}// iterator::operator=
			
			/*!
			 * Comparison operator
			 *
			 * \param it other iterator
			 */
			bool operator==(const iterator & it)
			{
				return _node == it._node;
			}// iterator::operator==
			
			/*!
			 * Comparison operator
			 *
			 * \param it other iterator
			 */
			bool operator!=(const iterator & it)
			{
				return _node != it._node;
			}// iterator::operator!=
			
			/*!
			 * tricky comparison operator comparing lexicographically w/ other key
			 * \param it other iterator
			 */
			bool operator<(const iterator & it)
			{
				if (*this == it)
					return false;
				else
				{
					if (it._node==NULL)//everything is < end()
						return true;
					else if (_node==NULL)
						return false;
					else if (valid () and it.valid ())
						return strcmp (_node->name, it.node->name)<0;
					else
						return false;// any of operands is invalid;
				}
			}//iterator::operator<
			
			/*!
			 * incrementation operator finds new element
			 */
			iterator & operator++()
			{
				_node = tree_find_next (_node);
				return *this;
			}
			
			friend class Dictionary<T>;
		};// class iterator
		
		iterator end ()
		{
			return iterator ();
		}
		
		iterator begin ()
		{
			return iterator (tree_find_next (
					reinterpret_cast<t_name_record *>(
					tree.first_vector)));
		}
		/*
		 * Add element into field w/ selected key. if any element was already placed
		 * there, it will be replaced w/ new one
		 * \param name key
		 * \param n new element
		 */
		void insert (const char * name, const T & n)
		{
			t_name_record * r = tree_add (name);
			if (r->datafield == NULL)
			{
				r->datafield = new T (n);
			}
			else
			{
				* ( r->datafield ) = n;
			}
		}// insert
		
		/*
		 * return iterator to specific node. If failed, return iterator to end()
		 * \note consider using contains(const char *), when need only to check,
		 * whenever element  exists.
		 */
		iterator find (const char * name)
		{
            // what happens when name == 0 ??
			return iterator (tree_find (name, tree.first_vector));
		}
		
		iterator partial_find (const char * name)
		{
			return iterator (tree_partial_find (name, tree.first_vector));
		}
		/*
		 * Nonstandard function tests if container contains specific key
		 *  (this equals "find(name)!=end()")
		 * \param name key
		 */
		bool contains (const char * name)
		{
			return tree_find (name, tree.first_vector) not_eq 0;
		}// contains
		
		/*
		 * Default constructor
		 * \exceptions std::bad_alloc when memory allocation fails
		 */
		Dictionary<T> ()
		{
			using namespace std;
			tree.max_num_occurrences=1;
			tree.first_vector=(t_name_vector*)calloc (1, sizeof(t_name_vector));
			if (!tree.first_vector)
				throw bad_alloc ();// memory allocationfailed
			tree.first_vector->type=T_VECTOR;
		}
		~Dictionary<T>()
		{
			tree_erase_vector (tree.first_vector);
		}
		
	};// class Dictionary
	
	//Add node to tree (more docummentation is placed with declaration)
	template<typename T>
		typename Dictionary<T>::t_name_record *
			Dictionary<T>::tree_add (const char * name)
	{
		using namespace std;
		int current = 0;
		t_name_vector * current_vector ;
		current_vector = tree.first_vector;
		while (1)
		{
			if (current_vector->record == 0)
			{
				/*create record */
				current_vector -> record = (t_name_record *)
						calloc (1, sizeof(t_name_record));
				if (! current_vector -> record )
					throw bad_alloc (); // C++ standard exception upon memory
				//  allocation failure
				current_vector -> record -> name =
						(char*) malloc (1+strlen (name));
				if (!current_vector -> record -> name)
					throw bad_alloc ();
				sprintf (current_vector -> record -> name, name);
				current_vector -> record -> type = T_RECORD;
				current_vector -> record -> num_occurrences = 1;
				current_vector -> record -> parent = current_vector;
				return current_vector -> record;
			}
			else if (current_vector->record->type==T_RECORD)
			{
				if (0==strcmp (current_vector->record ->name, name))
				{
					/*update record (already exists)*/
					current_vector -> record -> num_occurrences ++;
					tree.max_num_occurrences=
							max (tree.max_num_occurrences,
							current_vector -> record -> num_occurrences);
					return current_vector -> record;
				}
				else
				{   /*name differs: must reorganize structure.*/
					/*replace record (leaf) with new vector (of leaves)*/
					t_name_vector * new_vector ;
					new_vector =
							(t_name_vector*)calloc (1, sizeof(t_name_vector));
					if (!new_vector)
						throw bad_alloc ();
					new_vector->type=T_VECTOR;
					new_vector->parent=current_vector;
					current_vector->record->parent=new_vector;
					
					new_vector->vector[ vmark (
							static_cast<size_t>(current_vector->record ->
							name[1+current]))]=current_vector->record;
					current_vector -> record = (t_name_record*)new_vector;
					current++;
					current_vector = new_vector;
				}
			}
			else
			{
				/*go into tree*/
				current_vector = reinterpret_cast<t_name_vector *>
						(current_vector->record);
				current++;
			}
		}
		THROW (LogicError); /*function shouldn't go here*/
	}//tree_add
	
	template<typename T>
		typename Dictionary<T>::t_name_record *
			Dictionary<T>::tree_partial_find 
				(const char * name,	t_name_vector * current_vector, 
					size_t current)	const
	{
		using namespace std;
		size_t sl = strlen (name);
		while (1)
		{
			if (current_vector->record == 0)// key does not exist
			{
				return 0;
			}
			else if (current_vector->record->type==T_RECORD)
			{
				if (0==strcmp (current_vector->record ->name, name))
					return current_vector->record;
				else
					return 0;
			}
			else //if (current_vector->record->type==T_VECTOR)
			{
				current_vector = (t_name_vector *) current_vector->record;
				current++;
				if (current==sl)// non unique match (even complete) returns vec
				{
					return reinterpret_cast<t_name_record *>
							(current_vector);
				}
			}
		}
	}//tree_partial_find
	
	template<typename T>
		typename Dictionary<T>::t_name_record *
			Dictionary<T>::tree_find (const char * name,
				t_name_vector * current_vector, int current)
					const
	{
		t_name_record * r = tree_partial_find (name, tree.first_vector, current);
		if (not r)
			return 0;
		else
		{
			if (r->type==T_RECORD)
				return r;
			else
			{
				r =
						reinterpret_cast<t_name_vector *>(r) -> vector[vmark (0)];
				if (r == NULL)
					return NULL; // no such record
				else if (r->type == T_VECTOR)
					return NULL; // begining of name OK, but it is not the same.
				else
				{// note: this test is unnecessary and exceprion will never be
					// thrown. this serves rather as invariant assertion
					if (0 == strcmp (r->name, name))
					{
						return r;
					}
					else
						THROW (LogicError);
				}
			}
		}
	}//tree_find
	
	template<typename T>
		typename Dictionary<T>::t_name_record *
			Dictionary<T>::tree_find_next (t_name_record * r)
	{
		size_t i = 0;
		do
		{			
			if (r->type==T_VECTOR)
			{
				for (;i<num_characters; i++ )
				{
					if (reinterpret_cast<t_name_vector*>
							(r)->vector[vmark (i)])
					{//ascend
						r=reinterpret_cast<t_name_vector*>
								(r)->vector[vmark (i)];
						if (r->type==T_RECORD)// found record
							return r;
						else// found vector
						{// browse it from it's begining
							i=-1;
							continue;
						}
					}
				}
			}
			if (r->parent not_eq 0)
			{
				// increase i, to point to vector position AFTER current record
				for (i=0;r->parent->vector[vmark (i++)]!=r; )
					;
				// descend
				r=reinterpret_cast<t_name_record*>(r->parent);
			}
			else
			{
				return NULL;
			}
		}// break loop if r->parent=0, as we have found tree.first
		while (true);
		THROW (LogicError);
	}
	//tree_find_next
	
	template<typename T>
		void Dictionary<T>::tree_erase_vector (t_name_vector * v)
	{
		
		for (size_t i=0;i<num_characters; i++ )
		{
			if (v->vector[i] not_eq NULL)
			{
				if (v->vector[i]->type==T_VECTOR)
					tree_erase_vector (
							reinterpret_cast<t_name_vector *>
							(v->vector[i]));
				else
					tree_erase_record (v->vector[i]);
			}
		}
		//free(v->vector); // vector is static field
		free (v);
	}
	// tree_erase_vector
	
#undef vmark
#undef record
}
#endif	/* __DICTIONARY_H__ */

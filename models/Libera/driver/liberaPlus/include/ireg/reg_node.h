/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef REG_NODE_H
#define REG_NODE_H

#include <set>

#include "istd/log.h"
#include "istd/string.h"
#include "icfg/element.h"
#include "ireg/expression.h"
#include "ireg/tree_node.h"
#include "ireg/notification_data.h"

namespace ireg {

    using mci::ExposeHiddenRegistry;

    /**
     * @addtogroup register Repository interface for signals, configuration parameters, etc...
     * @{
     */

    /**
     * Specific implementation of a TreeNode to create a tree
     * in the local process.
     */
    class RegNode : public TreeNode {
    public:
        virtual ~RegNode();

        void SetValidator(const ExpressionPtr& a_expression);
        ExpressionPtr GetValidator() const;

        bool LoadConfiguration();
        bool WriteConfiguration();

        bool Deserialize(const icfg::ElementPtr &a_prop);
        icfg::ElementPtr Serialize();

        /**
        * Sets new value for non-constant nodes.
        *
        * Used when updating the node on the server to be able to update values
        * for read-only nodes as well.
        */
        template <class T>
            void Set(const T& v, size_t a_pos = 0)
            {
                auto l(TreeNode::Lock());

                try {
                    if (IsConstant()) {
                        throw istd::Exception("Node is a constant.");
                    }
                    ApplyAndEmitValue(v, a_pos);
                }
                catch (istd::Exception &e) {
                    istd_EXCEPTION("ireg: " << GetName() << ": " << e.what());
                }
            }

        /**
        * Sets new value for non-constant nodes.
        *
        * Used when updating the node on the server to be able to update values
        * for read-only nodes as well.
        */
        template <class T>
            bool SetValue(const T& v, size_t a_pos = 0) throw()
            __attribute__((warn_unused_result));

        /**
         * Verifies position and size for Set method against node's size
         */
        void VerifySetArgs(size_t a_pos, size_t a_size) const;

    protected:
        // RegNode can be created only with factory function
        RegNode();
        RegNode(const std::string& a_name, Flags a_flags = 0);

        mci_CREATE_NODE_FRIEND;

        // --- TreeNode interface ---

        using TreeNode::UniqueLock;
        using TreeNode::LockRead;

        virtual bool OnGetValidatorExpression(std::string &a_value,
                                              const Path& a_rp = c_noRelPath) const;
        virtual bool OnCleanup(const Path& a_rp);

        virtual void Get(std::string&, size_t, const Path&);
        virtual void Get(int64_t&, size_t, const Path&);
        virtual void Get(uint64_t&, size_t, const Path&);
        virtual void Get(int32_t&, size_t, const Path&);
        virtual void Get(uint32_t&, size_t, const Path&);
        virtual void Get(double&, size_t, const Path&);
        virtual void Get(float&, size_t, const Path&);
        virtual void Get(bool&, size_t, const Path&);

        virtual void Get(std::vector<std::string>&, size_t, size_t, const Path&);
        virtual void Get(std::vector<int64_t>&, size_t, size_t, const Path&);
        virtual void Get(std::vector<uint64_t>&, size_t, size_t, const Path&);
        virtual void Get(std::vector<int32_t>&, size_t, size_t, const Path&);
        virtual void Get(std::vector<uint32_t>&, size_t, size_t, const Path&);
        virtual void Get(std::vector<double>&, size_t, size_t, const Path&);
        virtual void Get(std::vector<float>&, size_t, size_t, const Path&);
        virtual void Get(std::vector<bool>&, size_t, size_t, const Path&);


        virtual void Set(const std::string &v, size_t a_pos, const Path&);
        virtual void Set(const int64_t &v, size_t a_pos, const Path&);
        virtual void Set(const uint64_t &v, size_t a_pos, const Path&);
        virtual void Set(const int32_t &v, size_t a_pos, const Path&);
        virtual void Set(const uint32_t &v, size_t a_pos, const Path&);
        virtual void Set(const double &v, size_t a_pos, const Path&);
        virtual void Set(const float &v, size_t a_pos, const Path&);
        virtual void Set(const bool &v, size_t a_pos, const Path&);

        virtual void Set(const std::vector<std::string> &v, size_t a_pos, const Path&);
        virtual void Set(const std::vector<int64_t> &v, size_t a_pos, const Path&);
        virtual void Set(const std::vector<uint64_t> &v, size_t a_pos, const Path&);
        virtual void Set(const std::vector<int32_t> &v, size_t a_pos, const Path&);
        virtual void Set(const std::vector<uint32_t> &v, size_t a_pos, const Path&);
        virtual void Set(const std::vector<double> &v, size_t a_pos, const Path&);
        virtual void Set(const std::vector<float> &v, size_t a_pos, const Path&);
        virtual void Set(const std::vector<bool> &v, size_t a_pos, const Path&);

        // --- End TreeNode interface ---

        template <class T>
            bool GetValue(T& v, size_t a_pos = 0) const throw()
            __attribute__((warn_unused_result));

        template <class T>
            bool GetValue(
                std::vector<T> &v, size_t a_pos = 0, size_t a_size = 0) const throw()
            __attribute__((warn_unused_result));

        /**
        * Verifies access rights to the node and then sets new value.
        */
       template <class T>
           void UpdateValue(const T& v, size_t a_pos = 0)
           {
               auto l(TreeNode::Lock());

               if (!IsWritable()) {
                  throw istd::Exception("Node is not writable.");
               }
               Set(v, a_pos);
           }


       /**
        * Unconditionally apply new value after performing validation
        * Used also when the node is manipulated internally,
        * for example when reading configuration from the file.
        * If new value is successfully applied, notification is sent
        *
        * @return success of changing the value
        */
       template <class T>
           void ApplyAndEmitValue(const T &v, size_t a_pos = 0)
       {
           ApplyValue(v, a_pos);
           EmitValue(v, a_pos);
       }

       virtual bool Emit();

       /**
        * Unconditionally apply new value after performing validation
        * Used also when the node is manipulated internally,
        * for example when reading configuration from the file.
        *
        * @return success of changing the value
        */
       template <class T>
           void ApplyValue(const T &v, size_t a_pos = 0)
       {
           _ApplyValue(&v, a_pos, 1);
       }

       template <class T>
           void ApplyValue(const std::vector<T> &v, size_t a_pos = 0)
       {
           if (v.size() == 0) {
               return;
           }
           _ApplyValue(&(v[0]), a_pos, v.size());
       }


       /**
        * Validates the value according to constraints of the node.
        * @throw ireg::ValidationError validation fails
        * @throw ireg::SyntaxError expression is not syntactically correct
        */
       template <class T>
           void Validate(const T& v) throw(ireg::ValidationError, ireg::SyntaxError)
           {
               if (!m_constraint) {
                   return;
               }
               if (!m_constraint->Evaluate(v)) {
                   throw ValidationError(m_constraint, istd::ToString(v));
               }
           }

       /**
        * Validates the value according to constraints of the node.
        * @throw ireg::ValidationError validation fails
        * @throw ireg::SyntaxError expression is not syntactically correct
        */
       template <class T>
           void Validate(const T* v, size_t a_pos, size_t a_size)
               throw(ireg::ValidationError, ireg::SyntaxError)
           {
               for (size_t i(0); i < a_size; ++i) {
                   Validate(v[i]);
               }
           }

       /**
        * Create notification data with type 'ValueChanged' and set v
        * Emit created notification data from this node.
        * @param v  changed value
        * @return true if notification is sent
        */
       template <class T>
           bool EmitValue(const T &v, size_t a_pos = 0)
       {
           auto l(TreeNode::Lock());

           // If array node send notification as array, otherwise as single value
           if (IsArray()) {
               std::vector<T> values = {v};
               return OnEmitValue(values, a_pos);
           }
           else {
               return OnEmitValue(v, a_pos);
           }
       }

       template <class T>
           bool EmitValue(const std::vector<T> &v, size_t a_pos = 0)
       {
           auto l(TreeNode::Lock());

           if (v.size() == 0) {
               return false;
           }

           // If array node send notification as array, otherwise as single value
           if (IsArray()) {
               return OnEmitValue(v, a_pos);
           }
           else {
               return OnEmitValue(v[0], a_pos);
           }
       }


       template <class T>
           void _ApplyValue(const T* v, size_t a_pos, size_t a_size)
           {
               auto l(TreeNode::Lock());

               VerifySetArgs(a_pos, a_size);

               // Will throw exception describing the cause
               Validate(v, a_pos, a_size);

               // Validation passed. Set value.
               OnSet(v, a_pos, a_size);
           }

       template <class T>
           bool _Emit();

       /**
        * General method for get node's values
        * @param v    Array of values returned from the node
        * @param a_pos      Starting position of element in node's array
        * @param a_size     Number of elements to return
        * @param a_rp  Relative path
        * @return Number of returned elements (<= a_size)
        */
       template <class T>
           size_t _Get(T *v, size_t a_pos, size_t a_size) const
           {
               UniqueLock l = LockRead();

               if (IsReadable()) {
                   size_t newSize = VerifyGetArgs(a_pos, a_size);
                   return OnGet(v, a_pos, newSize);
               }
               else {
                   throw istd::Exception("TreeNode is not readable.");
               }
           }

       /**
        * Template class for _Get is implemented for simpler specialization
        * of special cases (ex. vector of bools)
        */
       template <class T>
           size_t _Get(std::vector<T>& v, size_t a_pos = 0) const
           {
               return _Get(&v[0], a_pos, v.size());
           }


       /**
        * Verifies position and size for Get method against node's size
        * Returns new size which might be smaller than requested one.
        */
       size_t VerifyGetArgs(size_t a_pos, size_t a_size) const;

       // Notifications
       bool OnEmit(const NotificationData &a_data);
       bool OnEmitValue(const istd::Any &v, size_t a_pos);
       bool OnSubscribe(const ClientId &a_id, const Path& a_rp = c_noRelPath);

       bool OnUnsubscribe(
           const ClientId &a_id,
           const Path& a_rp = c_noRelPath,
           bool a_updateDispatcher = true);

       virtual bool OnDeserialize(const icfg::ElementPtr &a_prop);
       virtual icfg::ElementPtr OnSerialize();

       virtual bool OnLoadConfiguration();
       virtual bool OnWriteConfiguration();

       virtual size_t OnGet(std::string* a_out, size_t a_pos, size_t a_size) const;
       virtual size_t OnGet(int32_t*  a_out, size_t a_pos, size_t a_size) const;
       virtual size_t OnGet(uint32_t* a_out, size_t a_pos, size_t a_size) const;
       virtual size_t OnGet(int64_t*  a_out, size_t a_pos, size_t a_size) const;
       virtual size_t OnGet(uint64_t* a_out, size_t a_pos, size_t a_size) const;
       virtual size_t OnGet(double*   a_out, size_t a_pos, size_t a_size) const;
       virtual size_t OnGet(float*    a_out, size_t a_pos, size_t a_size) const;
       virtual size_t OnGet(bool*     a_out, size_t a_pos, size_t a_size) const;

       virtual void OnSet(const std::string* a_in, size_t a_pos, size_t a_size);
       virtual void OnSet(const int32_t*  a_in, size_t a_pos, size_t a_size);
       virtual void OnSet(const uint32_t* a_in, size_t a_pos, size_t a_size);
       virtual void OnSet(const int64_t*  a_in, size_t a_pos, size_t a_size);
       virtual void OnSet(const uint64_t* a_in, size_t a_pos, size_t a_size);
       virtual void OnSet(const double*   a_in, size_t a_pos, size_t a_size);
       virtual void OnSet(const float*    a_in, size_t a_pos, size_t a_size);
       virtual void OnSet(const bool*     a_in, size_t a_pos, size_t a_size);

    private:

       template <class T>
           void FetchValue(T& v, size_t a_pos = 0) const
           {
               try {
                   size_t num = _Get(&v, a_pos, 1);
                   if (num == 0) {
                       throw istd::Exception("Unable to get value.");
                   }
               }
              catch (istd::Exception &e) {
                  istd_EXCEPTION("ireg node " << GetName() << ": " << e.what());
              }
           }

       template <class T>
           void FetchValue(std::vector<T> &v, size_t a_pos = 0, size_t a_size = 0) const

           {
               try {
                   size_t size = (a_size == 0 ? GetSize() : a_size);
                   v.resize(size);
                   if (size == 0) {
                       // Array is empty or 0 elements requested.
                       return;
                   }

                   size_t num = _Get(v, a_pos);
                   if (num == 0) {
                       throw istd::Exception("Unable to get value.");
                   }
                   v.resize(num);
               }
               catch (istd::Exception &e) {
                   istd_EXCEPTION("ireg node " << GetName() << ": " << e.what());
               }
           }


       template <class T>
           void ReadConfigArray(const icfg::ElementPtr &a_prop);

       template <class T>
           void ReadConfigValue(const icfg::ElementPtr &a_prop);

       void SetConfigValue(const icfg::ElementPtr &a_prop,
                           const std::string &v,
                           icfg::DataType_e a_type);

       template <class T>
       void CreateConfigValue(const icfg::ElementPtr &a_prop,
                              icfg::DataType_e a_type);

       // Clients connected to this node's notifications
       std::set<ClientId>  m_clients;
       ExpressionPtr   m_constraint;
    };

    /// Explicit specialization for vector of booleans (special case)
    template<> void RegNode::ApplyValue<bool>(
        const std::vector<bool> &v, const size_t a_pos);


    /// Explicit specialization for vector of booleans (special case)
    template<> size_t RegNode::_Get<bool>(
        std::vector<bool> &v, const size_t a_pos) const;

    template <class T>
        bool RegNode::GetValue(T& v, const size_t a_pos) const throw()
        {
            try {
                FetchValue(v, a_pos);
            }
            catch (istd::Exception &ex) {
                istd_TRC(istd::eTrcLow, "Cannot get value " << ex.what());
                return false;
            }
            return true;
        }

    template <class T>
        bool RegNode::GetValue(
            std::vector<T> &v, const size_t a_pos, const size_t a_size) const throw()
        {
            try {
                FetchValue(v, a_pos, a_size);
            }
            catch (istd::Exception &ex) {
                istd_TRC(istd::eTrcLow, "Cannot get value " << ex.what());
                return false;
            }
            return true;
        }

    template <class T>
        bool RegNode::SetValue(const T& v, const size_t a_pos) throw()
        {
            try {
                Set(v, a_pos);
            }
            catch (istd::Exception &ex) {
                istd_TRC(istd::eTrcLow, "Cannot set value " << ex.what());
                return false;
            }
            return true;
        }

    /**@}*/

} // namespace

namespace mci {

    namespace factory {
        /**
         * Helper factory function to create the node and assign the validator,
         * passed as the first argument of the function.
         *
         * Example:
         * @verbatim
    auto tnv = Create<RegValueInt32Node>(
        ireg::Expr<Gt<int32_t>>(5),
        "node_with_validator", 10
    );
         * @endverbatim
         * @see mci::Create
         */
        template<typename NODE_TYPE, typename... ARGS>
            TreeNodePtr Create(ireg::ExpressionPtr a_expression, ARGS&& ...args)
            {
                static_assert(
                    std::is_base_of<ireg::RegNode, NODE_TYPE>::value,
                    "NODE_TYPE must be derived from RegNode.");

                TreeNodePtr tn(Create<NODE_TYPE>(std::forward<ARGS>(args)...) );
                auto rn = std::dynamic_pointer_cast<ireg::RegNode>(tn);
                rn->SetValidator(a_expression);

                return tn;
            }
    }

}

#endif /* REG_NODE_H */

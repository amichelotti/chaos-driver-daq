/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: remote_node.h 20696 2014-06-20 09:22:35Z damijan.skvarc $
 */


#ifndef MCI_REMOTE_NODE_H_
#define MCI_REMOTE_NODE_H_


#include <set>

#include "ireg/tree_node.h"

// Replace with "GCC diagnostic push" and "GCC diagnostic pop"
// in gcc 4.5 
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_application.h"
#include "inet/i_remote_node.h"
#include "inet/i_remote_signal.h"
#pragma GCC diagnostic warning "-Wold-style-cast"


namespace mci {

    class i_Notification_impl;

    /**
     * @addtogroup register
     * @{
     */
    /**
     * Specific implementation of a TreeNode to
     * connect to a remote tree. Implements client side of the proxy.
     * @see i_RemoteNode_impl
     */
    class RemoteNode: public TreeNode {
    public:
        typedef std::set<std::string> InterfaceVersions;
        static const InterfaceVersions c_SupportedInterface;
        /**
         * Name of the node is generated automatically from hostname.
         */
        RemoteNode(
                const std::string&  a_host,
                unsigned int        a_port,
                const std::string&  a_appName,
                InterfaceVersions   a_interfaceVersions  = c_SupportedInterface);

        RemoteNode(
            const std::string&  a_name,
            const std::string&  a_host,
            unsigned int        a_port,
            const std::string&  a_appName,
            InterfaceVersions   a_interfaceVersions = c_SupportedInterface);

        virtual bool Init();

        virtual ~RemoteNode();

        virtual Flags OnGetFlags(const Path& a_rp) const;
        virtual Flags OnSetFlags(Flags a_flags, const Path& a_relPath);

        virtual NodeValType_e OnGetValueType(const Path& a_rp) const;

        bool Reinit();

        i_NET::RemoteSignal_ptr GetRemoteSignal(const Path& a_rp);

        std::string             GetInterfaceVersion();

    protected:

        // TODO: Implement internal interface to query all remote nodes
        virtual TreeNodeVec GetAllNodes();

        bool OnSubscribe(const ClientId &a_id,
                               const Path& a_rp);
        bool OnUnsubscribe(const ClientId &a_id,
                               const Path& a_rp,
                               bool a_updateDispatcher = true);

        virtual size_t OnGetSize(const Path& a_rp) const;


        // --- TreeNode interface ---

        virtual void Get(std::string&, const size_t, const Path&);
        virtual void Get(int64_t&, const size_t, const Path&);
        virtual void Get(uint64_t&, const size_t, const Path&);
        virtual void Get(int32_t&, const size_t, const Path&);
        virtual void Get(uint32_t&, const size_t, const Path&);
        virtual void Get(double&, const size_t, const Path&);
        virtual void Get(float&, const size_t, const Path&);
        virtual void Get(bool&, const size_t, const Path&);

        virtual void Get(std::vector<std::string>&, const size_t, const size_t, const Path&);
        virtual void Get(std::vector<int64_t>&, const size_t, const size_t, const Path&);
        virtual void Get(std::vector<uint64_t>&, const size_t, const size_t, const Path&);
        virtual void Get(std::vector<int32_t>&, const size_t, const size_t, const Path&);
        virtual void Get(std::vector<uint32_t>&, const size_t, const size_t, const Path&);
        virtual void Get(std::vector<double>&, const size_t, const size_t, const Path&);
        virtual void Get(std::vector<float>&, const size_t, const size_t, const Path&);
        virtual void Get(std::vector<bool>&, const size_t, const size_t, const Path&);


        virtual void Set(const std::string &v, const size_t a_pos, const Path&);
        virtual void Set(const int64_t &v, const size_t a_pos, const Path&);
        virtual void Set(const uint64_t &v, const size_t a_pos, const Path&);
        virtual void Set(const int32_t &v, const size_t a_pos, const Path&);
        virtual void Set(const uint32_t &v, const size_t a_pos, const Path&);
        virtual void Set(const double &v, const size_t a_pos, const Path&);
        virtual void Set(const float &v, const size_t a_pos, const Path&);
        virtual void Set(const bool &v, const size_t a_pos, const Path&);

        virtual void Set(const std::vector<std::string> &v, const size_t a_pos, const Path&);
        virtual void Set(const std::vector<int64_t> &v, const size_t a_pos, const Path&);
        virtual void Set(const std::vector<uint64_t> &v, const size_t a_pos, const Path&);
        virtual void Set(const std::vector<int32_t> &v, const size_t a_pos, const Path&);
        virtual void Set(const std::vector<uint32_t> &v, const size_t a_pos, const Path&);
        virtual void Set(const std::vector<double> &v, const size_t a_pos, const Path&);
        virtual void Set(const std::vector<float> &v, const size_t a_pos, const Path&);
        virtual void Set(const std::vector<bool> &v, const size_t a_pos, const Path&);

        // --- End TreeNode interface ---

        virtual bool OnGetDomainValues(istd::StrTokens &v, const Path& a_rp) const;
        virtual bool OnGetValidatorExpression(std::string &a_value,
                                              const Path& a_rp = c_noRelPath) const;
        bool OnCleanup(const Path& a_rp);

    private:
        std::string                 m_uri;
        i_NET::RemoteNode_var       m_remote;
        i_NET::Application_var      m_remoteApp;
        i_Notification_impl*        m_notification;
        InterfaceVersions           m_interfaceVersions;
    };
    /**@}*/

    namespace factory {

        template<> struct Factory<RemoteNode> {
            template<typename ...ARGS>
                static inline TreeNodePtr New(ARGS&& ...args)
                {
                    return Create<RemoteNodeStructure, RemoteNode>(std::forward<ARGS>(args)...);
                }
        };

    } // namespace
}

#endif /* MCI_REMOTE_NODE_H_ */

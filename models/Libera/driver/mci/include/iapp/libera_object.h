/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_object.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef LIBERA_OBJECT_H
#define LIBERA_OBJECT_H

#include <memory>
#include <typeinfo>
#include "istd/trace.h"
#include "ireg/declarations.h"

#include "declarations.h"

namespace iapp {

    class LiberaObject : public std::enable_shared_from_this<LiberaObject>
    {
    public:
        LiberaObject(LiberaObject& a_parent, const std::string &a_name);
        virtual  ~LiberaObject();

        /* prevent copy constructor and assignment */
        LiberaObject(const LiberaObject& a_other) = delete;
        LiberaObject& operator = (const LiberaObject& a_other) = delete;

        const std::string&         GetName() const;
        std::string                GetDecoratedName(const std::string& a_postfix);

        inline LiberaObjectPtr     SharedPtr()           { return this->shared_from_this(); }
        LiberaObjectPtr            GetChild(int a_child) { return m_children.at(a_child); }
        const LiberaObjects&       GetChildren() const   { return m_children; }

        LiberaObjectPtr            GetParent() const     { return m_parent.lock();     }
        LiberaObjectPtr            GetAncestor(const std::type_info &a_type) const;

        LiberaObjectPtr            DetachChild(LiberaObject *a_child);
        void                       RemoveChild(LiberaObject *a_child);

        bool                       GetObjects(const std::type_info &a_type, LiberaObjects &a_objects);
        ApplicationState_e         GetApplicationState() const;

        void                       SetRegistryFlags(ireg::Flags a_reg_flags);
        void                       AddRegistry(ireg::TreeNodePtr &a_parent, const char *a_name = NULL);
        void                       PreInit();
        void                       PostInit();
        void                       PreConfig();
        void                       PostConfig();
        void                       StartRunning();
        void                       AcceptRequests();
        void                       Shutdown();
        bool                       DeleterBlocked()   { return m_deleter_blocked; }
        ireg::TreeNodePtr          GetParentRegNode() { return m_parent_reg_node.lock(); }

    protected:
        virtual void               OnRegistryAdd(ireg::TreeNodePtr &a_parent) {};
        virtual void               OnPreInit() {};
        virtual void               OnPostInit() {};
        virtual void               OnPreConfig() {};
        virtual void               OnPostConfig() {};
        virtual void               OnStartRunning() {};
        virtual void               OnAcceptRequests() {};
        virtual void               OnShutdown() {};
        virtual bool               OnQueryChild(const LiberaObject &a_child);
        virtual const std::string& OnGetName() const;

        void                       UpdateChildrenParent();

    protected:
        template<typename OBJ_TYPE, typename T, typename ...ARGS>
        friend typename std::enable_if<std::is_base_of<LiberaObject,OBJ_TYPE>::value, OBJ_TYPE>::type *
        Create(T &a_parent, ARGS&& ...a_args);

    private:

        void                       AddChild(LiberaObject *a_child);
        void                       QueryChild(LiberaObject *a_child);

        void                       SetParent(LiberaObject& a_parent);
        void                       AddToParent(LiberaObject& a_parent);

        std::string                m_name;
        ireg::Flags                m_reg_flags;
        LiberaObjects              m_children;
        LiberaObjectWeakPtr        m_parent;
        ireg::TreeNodeWeakPtr      m_parent_reg_node;
        bool                       m_registry_created;
        bool                       m_deleter_blocked;
    };

    /**
     * Creates any class of LiberaObject and adds it into a_parent as shared
     * pointer
     * @param a_parent Parent LiberaObject class
     * @param a_args   Any arguments (must match the constructor of OBJ_TYPE
     * @return pointer to created class or throws exception if not successful.
     */
    template<typename OBJ_TYPE, typename T, typename ...ARGS>
    typename std::enable_if<std::is_base_of<LiberaObject,OBJ_TYPE>::value, OBJ_TYPE>::type *
    Create(T &a_parent, ARGS&& ...a_args)
    {
        OBJ_TYPE* obj(new OBJ_TYPE(a_parent, std::forward<ARGS>(a_args)...));
        a_parent.QueryChild(obj);
        return obj;
    }

} // namespace


#endif // LIBERA_OBJECT_H

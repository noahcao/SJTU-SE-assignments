package com.entity;

import com.opensymphony.xwork2.ActionSupport;
import org.hibernate.Session;

import java.util.List;

public class UserList  extends ActionSupport {
    private List<UserAdmin> users;

    public List<UserAdmin> getUsers() {
        return users;
    }

    public void setUsers(List<UserAdmin> users) {
        this.users = users;
    }

    @SuppressWarnings("unchecked")
    public String queryUserList() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<UserAdmin> result = session.createQuery("from UserAdmin").list();
        setUsers(result);
        session.getTransaction().commit();
        session.close();
        return "success";
    }
}

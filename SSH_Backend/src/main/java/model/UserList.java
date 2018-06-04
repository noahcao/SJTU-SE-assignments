package model;

import com.opensymphony.xwork2.ActionSupport;
import org.hibernate.Session;

import java.util.List;

public class UserList  extends ActionSupport {
    private List<User> users;

    public List<User> getUsers() {
        return users;
    }

    public void setUsers(List<User> users) {
        this.users = users;
    }

    @SuppressWarnings("unchecked")
    public String queryUserList() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<User> result = session.createQuery("from User").list();
        setUsers(result);
        session.getTransaction().commit();
        session.close();
        return "success";
    }
}

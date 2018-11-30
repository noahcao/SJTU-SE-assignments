package dao.impl;

import java.util.List;

import model.User;

import org.springframework.orm.hibernate5.support.HibernateDaoSupport;

import dao.UserDao;

public class UserDaoImpl extends HibernateDaoSupport implements UserDao {

    public Integer save(User user) {
        return (Integer) getHibernateTemplate().save(user);
    }

    public void delete(User user) {
        getHibernateTemplate().delete(user);
    }

    public void update(User user) {
        getHibernateTemplate().merge(user);
    }

    public User getUserById(int id) {
        @SuppressWarnings("unchecked")
        List<User> users = (List<User>) getHibernateTemplate().find(
                "from User as u where u.id=?", id);
        User user = users.size() > 0 ? users.get(0) : null;
        return user;
    }

    public User queryUser(String username, String password) {
        @SuppressWarnings("unchecked")
        List<User> users = (List<User>) getHibernateTemplate().find(
                "from User as u where u.username=? and u.password=?", username, password);
        User user = users.size() > 0 ? users.get(0) : null;
        return user;
    }

    public List<User> getAllUsers() {
        @SuppressWarnings("unchecked")
        List<User> users = (List<User>) getHibernateTemplate()
                .find("from User");
        System.out.println("USER:");
        System.out.println(users);
        return users;
    }

    public User getUserByName(String username) {
        @SuppressWarnings("unchecked")
        List<User> users = (List<User>) getHibernateTemplate().find(
                "from User as u where u.username=?", username);
        System.out.println("-----------------------------**********-----------------");
        System.out.println(username);
        System.out.println("------------------------------********----------------");
        User user = users.size() > 0 ? users.get(0) : null;
        return user;
    }

}

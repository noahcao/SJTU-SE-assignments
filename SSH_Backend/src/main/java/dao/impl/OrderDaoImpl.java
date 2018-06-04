package dao.impl;

import model.Order;

import java.util.List;

import org.springframework.orm.hibernate5.support.HibernateDaoSupport;

import dao.OrderDao;

public class OrderDaoImpl extends HibernateDaoSupport implements OrderDao{
    public Integer save(Order order) {
        return (Integer) getHibernateTemplate().save(order);
    }

    public void delete(Order order) {
        getHibernateTemplate().delete(order);
    }

    public void update(Order order) {
        getHibernateTemplate().merge(order);
    }

    public Order getOrderById(int id) {
        @SuppressWarnings("unchecked")
        List<Order> orders = (List<Order>) getHibernateTemplate().find(
                "from Order as o where o.id=?", id);
        Order order = orders.size() > 0 ? orders.get(0) : null;
        return order;
    }

    public List<Order> getOrdersByUserId(int userid){
        @SuppressWarnings("unchecked")
        List<Order> orders = (List<Order>) getHibernateTemplate().find(
                "from Order as o where o.userid=?", userid);
        return orders;
    }

    public List<Order> getAllOrders() {
        @SuppressWarnings("unchecked")
        List<Order> orders = (List<Order>) getHibernateTemplate().find(
                "from Order");
        return orders;
    }
}

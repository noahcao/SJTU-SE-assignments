package dao;

import java.util.List;

import model.Order;

public interface OrderDao {
    public Integer save(Order oder);

    public void delete(Order order);

    public void update(Order order);

    public Order getOrderById(int id);

    public List<Order> getAllOrders();

    public List<Order> getOrdersByUserId(int userid);
}

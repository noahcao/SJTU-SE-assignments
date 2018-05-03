package com.entity;

import com.opensymphony.xwork2.ActionSupport;
import org.hibernate.Session;

import java.util.List;

public class GoodList extends ActionSupport {
    private List<Good> goods;

    public List<Good> getGoods() {
        return goods;
    }

    public void setGoods(List<Good> goods) {
        this.goods = goods;
    }

    @SuppressWarnings("unchecked")
    public String queryGoodList() throws Exception {
        Session session = HibernateUtil.getSessionFactory().getCurrentSession();
        session.beginTransaction();
        List<Good> result = session.createQuery("from Good").list();
        setGoods(result);
        session.getTransaction().commit();
        session.close();
        return "success";
    }

}

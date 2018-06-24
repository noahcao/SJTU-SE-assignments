package Icon.dao.impl;

import java.util.List;
import java.util.Set;

import Icon.dao.IconDao;
import Icon.model.IconEntity;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Sort;
import org.springframework.data.domain.Sort.Direction;
import org.springframework.data.domain.Sort.Order;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.query.Criteria;
import org.springframework.data.mongodb.core.query.Query;
import org.springframework.data.mongodb.core.query.Update;
import org.springframework.stereotype.Repository;

import com.mongodb.DB;

@Repository
public class IconDaoImpl implements IconDao {

    @Autowired
    private MongoTemplate mongoTemplate;

    @Override
    public void _test() {
        Set<String> colls = this.mongoTemplate.getCollectionNames();
        for (String coll : colls) {
            System.out.println("CollectionName=" + coll);
        }
        DB db = this.mongoTemplate.getDb();
        System.out.println("db=" + db.toString());
    }

    @Override
    public void createCollection() {
        if (!this.mongoTemplate.collectionExists(IconEntity.class)) {
            this.mongoTemplate.createCollection(IconEntity.class);
        }
    }

    @Override
    public List<IconEntity> findList(int skip, int limit) {
        Query query = new Query();
        query.with(new Sort(new Order(Direction.ASC, "_id")));
        query.skip(skip).limit(limit);
        return this.mongoTemplate.find(query, IconEntity.class);
    }

    @Override
    public IconEntity findOne(int id) {
        System.out.println("user id is: " + id);
        Query query = new Query();
        query.addCriteria(new Criteria("_id").is(id));
        return this.mongoTemplate.findOne(query, IconEntity.class);
    }

    @Override
    public void insert(IconEntity entity) {
        System.out.println("try tooooooo insert a user img");
        this.mongoTemplate.insert(entity);
        Query query = new Query();
        query.addCriteria(new Criteria("_id").is(entity.getId()));
        System.out.println( this.mongoTemplate.findOne(query, IconEntity.class).getIcon());
    }

    @Override
    public void update(IconEntity entity) {
        Query query = new Query();
        query.addCriteria(new Criteria("_id").is(entity.getId()));
        IconEntity iconInside = findOne(entity.getId());
        if(iconInside == null){
            System.out.print("No a img inside alresade");
            insert(entity);
        }
        else{
            Update update = new Update();
            update.set("icon", entity.getIcon());
            this.mongoTemplate.updateFirst(query, update, IconEntity.class);
        }
    }

}

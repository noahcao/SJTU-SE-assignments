package Icon.dao;

import java.util.List;

import Icon.model.IconEntity;

import org.springframework.transaction.annotation.Transactional;

@Transactional
public interface IconDao {

    public abstract void _test();

    public abstract void createCollection();

    public abstract List<IconEntity> findList(int skip, int limit);

    public abstract IconEntity findOne(int id);

    public abstract void insert(IconEntity entity);

    public abstract void update(IconEntity entity);

}
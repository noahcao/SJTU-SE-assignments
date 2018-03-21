  <template>
    <div id="main">
      <div class="row mx-auto w-75" id="topBlock">

        <div class="col-6">
          <div class="btn-group">
            <button type="button" class="btn btn-primary" >Search</button>
            <button type="button" class="btn btn-primary" data-toggle="modal" data-target="#myModal">新增</button>
            <button type="button" class="btn btn-primary" @click="saveRows">保存</button>
            <button type="button" class="btn btn-primary" @click="delRows">删除</button>
          </div>
        </div>

        <div class="col-6">
          <div class="input-group">
            <input type="text" class="form-control input-group-sm" placeholder="输入设备编号进行搜索">
            <span class="input-group-btn">
              <button class="btn btn-default" type="button"><i class="fa fa-search"></i></button>
            </span>

          </div>
        </div>

      </div>

      <div class="row mx-auto w-75">
        <div class="col-12">
          <table class="table table-hover table-success">
            <thead class="thead-default">
            <tr>
              <th><input type="checkbox"></th>
              <th>序号</th>
              <th>设备编号</th>
              <th>设备名称</th>
              <th>设备状态</th>
              <th>采购日期</th>
            </tr>
            </thead>
            <tbody>
            <tr v-for="data in datas">
              <td><input type="checkbox" :value="index" v-model="checkedRows"></td>
              <td>{{data.code}}</td>
              <td>{{data.name}}</td>
              <td>{{data.states}}</td>
              <td>{{data.data}}</td>
              <td>{{data.admin}}</td>
            </tr>
            </tbody>
          </table>
        </div>
      </div>

      <table data-toggle="table">
        <thead>
        <tr>
          <th>Item ID</th>
          <th>Item Name</th>
          <th>Item Price</th>
        </tr>
        </thead>
        <tbody>
        <tr>
          <td>1</td>
          <td>Item 1</td>
          <td>$1</td>
        </tr>
        <tr>
          <td>2</td>
          <td>Item 2</td>
          <td>$2</td>
        </tr>
        </tbody>
      </table>


    </div>

  </template>

  <script>
    import Vue from 'vue'
    export default{
        name: 'homapage',
        data (){
            return {
                datas : [
                  {
                    code: "A2017-001",
                    name: "3800充电器",
                    states: "正常",
                    date: "2017-01-21",
                    admin: "andy"
                  },
                  {
                    code: "A2017-002",
                    name: "Lenovo Type-c转接器",
                    states: "正常",
                    date: "2017-01-21",
                    admin: "zero"
                  }
                ],
          tableData: [
            {"name":"赵伟","tel":"156*****1987","hobby":"钢琴、书法、唱歌","address":"上海市黄浦区金陵东路569号17楼"},
            {"name":"李伟","tel":"182*****1538","hobby":"钢琴、书法、唱歌","address":"上海市奉贤区南桥镇立新路12号2楼"},
            {"name":"孙伟","tel":"161*****0097","hobby":"钢琴、书法、唱歌","address":"上海市崇明县城桥镇八一路739号"},
            {"name":"周伟","tel":"197*****1123","hobby":"钢琴、书法、唱歌","address":"上海市青浦区青浦镇章浜路24号"},
            {"name":"吴伟","tel":"183*****6678","hobby":"钢琴、书法、唱歌","address":"上海市松江区乐都西路867-871号"}
          ],
              columns: [
                {
                  field: 'custome', title:'序号', width: 50, titleAlign: 'center', columnAlign: 'center',
                  formatter: function (rowData,rowIndex,pagingIndex,field) {
                    return rowIndex < 3 ? '<span style="color:red;font-weight: bold;">' + (rowIndex + 1) + '</span>' : rowIndex + 1
                  }, isFrozen: true,isResize:true
                },
                {field: 'name', title:'姓名', width: 80, titleAlign: 'center',columnAlign:'center',isResize:true},
                {field: 'tel', title: '手机号码', width: 150, titleAlign: 'center',columnAlign:'center',isResize:true},
                {field: 'hobby', title: '爱好', width: 150, titleAlign: 'center',columnAlign:'center',isResize:true},
                {field: 'address', title: '地址', width: 230, titleAlign: 'center',columnAlign:'left',isResize:true},
                {field: 'custome-adv', title: '操作',width: 200, titleAlign: 'center',columnAlign:'center',componentName:'table-operation',isResize:true}
              ]


            }
        },
      methods:{
        customCompFunc(params){

          console.log(params);

          if (params.type === 'delete'){ // do delete operation

            this.$delete(this.tableData,params.index);

          }else if (params.type === 'edit'){ // do edit operation

            alert(`行号：${params.index} 姓名：${params.rowData['name']}`)
          }

        }
      }
    }


    Vue.component('table-operation',{
      template:`<span>
        <a href="" @click.stop.prevent="update(rowData,index)">编辑</a>&nbsp;
        <a href="" @click.stop.prevent="deleteRow(rowData,index)">删除</a>
        </span>`,
      props:{
        rowData:{
          type:Object
        },
        field:{
          type:String
        },
        index:{
          type:Number
        }
      },
      methods:{
        update(){

          // 参数根据业务场景随意构造
          let params = {type:'edit',index:this.index,rowData:this.rowData};
          this.$emit('on-custom-comp',params);
        },

        deleteRow(){

          // 参数根据业务场景随意构造
          let params = {type:'delete',index:this.index};
          this.$emit('on-custom-comp',params);

        }
      }
    })

  </script>

  <style>
    #main {
      padding: 10px;
      background-color: lightcyan;
      margin: 10px;
      height: 1000px;
    }

    .btn-primary{
      margin: 10px;
      height: 30px;
      width: 60px;
    }
    #topBlock{
      height: 100px;
      width: 95%;
      border: solid;
      background-color: red;
    }
  </style>

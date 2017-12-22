<template>

<div class="container" id="showBoard">

      <div class="row" style="background: rgba(255,255,255,0)">

        <div class="col-xs-4" id="studyboard">
          <div class="card" style="border-radius: 10px">
            <div class="card-block" style="border-radius: 8px;">
              <h4 class="card-title" id="studyboardtitle">约学习</h4>
            </div>
          </div>
          <div class="modal-body" >
            <div v-for="poster in Posters.studyPoster">
              <div class="card"
              style="border: solid orangered; border-radius: 10px;width: 100%;margin: 10px; background-color: lightsalmon"
              v-on:mouseenter="showContent(poster.name)" v-on:mouseleave="hideContent">
              <div class="card-block" style="border-radius: 10px">
                <h6 class="card-title" style="background-color: lightsalmon;padding-bottom: 2px">{{poster.title}}</h6>
                <div class="card-text" style="border-bottom-right-radius: 10px;border-bottom-left-radius: 10px">
                  {{poster.month}}/{{poster.day}}  {{poster.start_hour}}:{{poster.start_minu}}--{{poster.end_hour}}:{{poster.end_minu}}
                </div>
              </div>
            </div>
          </div>
        </div>
      </div>


      <div class="col-xs-4" id="sportboard">
        <div class="card" style="border-radius: 10px;">
          <div class="card-block" style="border-radius: 8px;">
            <h4 class="card-title" id="sportboardtitle">约运动</h4>
          </div>
        </div>
        <div class="modal-body" >
          <div v-for="poster in Posters.sportPoster">
            <div class="card"
            style="border: solid cornflowerblue; border-radius: 10px;width: 100%;margin: 10px; background-color: lightcyan"
            v-on:mouseenter="showContent(poster.name)" v-on:mouseleave="hideContent">
            <div class="card-block" style="border-radius: 10px">
              <h6 class="card-title" style="background-color: lightcyan;padding-bottom: 2px">{{poster.title}}</h6>
              <div class="card-text" style="border-bottom-right-radius: 10px;border-bottom-left-radius: 10px">
                {{poster.month}}/{{poster.day}}  {{poster.start_hour}}:{{poster.start_minu}}--{{poster.end_hour}}:{{poster.end_minu}}
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>


    <div class="col-xs-4" id="outdoorboard">
      <div class="card" style="border-radius: 10px;">
        <div class="card-block" style="border-radius: 8px;">
          <h4 class="card-title" id="outdoorboardtitle">约户外</h4>
        </div>
      </div>
      <div class="modal-body" >
        <div v-for="poster in Posters.outdoorPoster">
          <div class="card"
          style="border: solid green; border-radius: 10px;width: 100%;margin: 10px; background-color: lightgreen"
          v-on:mouseenter="showContent(poster.name)" v-on:mouseleave="hideContent">
          <div class="card-block" style="border-radius: 10px">
            <h6 class="card-title" style="background-color: lightgreen;padding-bottom: 2px">{{poster.title}}</h6>
            <div class="card-text" style="border-bottom-right-radius: 10px;border-bottom-left-radius: 10px">
              {{poster.month}}/{{poster.day}}  {{poster.start_hour}}:{{poster.start_minu}}--{{poster.end_hour}}:{{poster.end_minu}}
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
  </div>
  </div>
  </template>

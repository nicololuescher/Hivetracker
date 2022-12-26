import { Component, OnInit } from '@angular/core';
import { Chart, registerables } from 'chart.js/auto';
import 'chartjs-adapter-moment';
import { HivedataService } from '../hivedata.service';
Chart.register(...registerables)

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css']
})
export class DashboardComponent implements OnInit {

  constructor(private _HiveDataService: HivedataService) { }

  ngOnInit(): void {
    let sets: any = [];
    this._HiveDataService.getHiveData().subscribe((data: any) => {
      let set: any = { data: [], label: 'Temperature' };
      data.temperature.forEach((element: any) => {
        set.data.push({ x: element.createdAt, y: element.temperature });
      });
      sets.push(set);
      set = { data: [], label: 'Weight' };
      data.weight.forEach((element: any) => {
        set.data.push({ x: element.createdAt, y: element.weight / 1000 });
      });
      sets.push(set);
      console.log(sets);
      var myChart = new Chart("chart", {
        type: 'line',
        options: {
          scales: {
              x: {
                  type: 'time'
              }
          }
        },
        data: {
          datasets: sets
        }
      });
    });

  }

}

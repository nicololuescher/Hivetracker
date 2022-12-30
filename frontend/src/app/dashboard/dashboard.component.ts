import { Component, OnInit } from '@angular/core';
import { Chart, registerables } from 'chart.js/auto';
import { Interval } from 'chart.js/dist/scales/scale.time';
import 'chartjs-adapter-moment';
import { HivedataService } from '../hivedata.service';
Chart.register(...registerables)

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css']
})
export class DashboardComponent implements OnInit {
  chart: Chart | null;
  sets: any;

  constructor(private _HiveDataService: HivedataService) {
    this.chart = null;
    this.sets = [];
  }

  async ngOnInit(): Promise<void> {
    Chart.defaults.color = "#fff";
    this.createChart(await this.fetchData());
    setInterval(async () => {
        this.updateChart(await this.fetchData());
    }, 1000);
  }

  fetchData(): any {
    return new Promise((resolve) => {
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
        this.sets = sets;
        resolve(sets);
      });
    });
  }

  createChart(sets: any): void {
    this.chart = new Chart("chart", {
      type: 'line',
      options: {
        scales: {
          x: {
            type: 'time'
          }
        },
        animations: {
          y: {
            duration: 0
          }
        }
      },
      data: {
        datasets: sets
      }
    });
  }

  updateChart(sets: any): void {
    if (this.chart) {
      console.log();
      if(this.chart.data.datasets[0].data.length == sets[0].data.length) return;
      this.chart.data.datasets = sets;
      this.chart.update();
    }
  }

}

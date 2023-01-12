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
  tempChart: Chart | null;
  weightChart: Chart | null;
  sets: any;
  chartOptions: object;

  constructor(private _HiveDataService: HivedataService) {
    this.tempChart = null;
    this.weightChart = null;
    this.sets = [];
    this.chartOptions = {
      scales: {
        x: {
          type: 'time'
        }
      },
      plugins: {
        legend: {
          display: false
        },
        tooltip: {
          mode: 'index',
          intersect: false,
          displayColors: false
        }
      },
      hover: {
        mode: 'nearest',
        intersect: false
      },
      maintainAspectRatio: false,
      animation: false
    }
  }

  async ngOnInit(): Promise<void> {
    Chart.defaults.color = "#fff";
    this.createChart(await this.fetchData());
    console.log(this.updateChart);

    setInterval(async () => {
      this.updateChart(await this.fetchData());
    }, 10000);
  }

  fetchData(): any {
    return new Promise((resolve) => {
      this._HiveDataService.getHiveData().subscribe((data: any) => {

        let tempSet: any = { data: [], label: 'Temperature', borderColor: '#fbbf24' };
        data.temperature.forEach((element: any) => {
          tempSet.data.push({ x: element.createdAt, y: element.temperature });
        });
        let weightSet: any = { data: [], label: 'Weight', borderColor: '#fbbf24' };
        data.weight.forEach((element: any) => {
          weightSet.data.push({ x: element.createdAt, y: element.weight / 1000 });
        });
        weightSet.data.sort(this.compare)
        tempSet.data.sort(this.compare)
        this.sets = { temperature: tempSet, weight: weightSet };
        resolve({ temperature: tempSet, weight: weightSet });
      });
    });
  }

  compare(a: any, b: any) {
    let d1 = Date.parse(a.x);
    let d2 = Date.parse(b.x);
    console.log(d1);
    if (d1 < d2) return -1;
    if (d1 > d2) return 1;
    return 0;
  }

  createChart(sets: any): void {
    this.tempChart = new Chart("tempChart", {
      type: 'line',
      options: this.chartOptions,
      data: {
        datasets: [sets.temperature]
      }
    });

    this.weightChart = new Chart("weightChart", {
      type: 'line',
      options: this.chartOptions,
      data: {
        datasets: [sets.weight]
      }
    });
  }

  updateChart(sets: any): void {
    if (this.tempChart && this.weightChart) {
      this.tempChart.data.datasets = [sets.temperature];
      this.tempChart.update();
      this.weightChart.data.datasets = [sets.weight];
      this.weightChart.update()
    }
  }

  zeroScale(): void {
    this._HiveDataService.setOffset(1).subscribe((data: any) => { });
  }

  setScale(): void {
    let input: HTMLInputElement = document.getElementById("trueWeight") as HTMLInputElement;

    if (input.value != "") {
      this._HiveDataService.setScale(1, Number(input.value)).subscribe((data: any) => {
        this.fetchData()
          .then((sets: any) => {
            this.updateChart(sets);
          });
      });

    }
  }
}

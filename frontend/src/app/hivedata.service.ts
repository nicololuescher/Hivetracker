import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class HivedataService {

  constructor(private _http:HttpClient) {  }

  getHiveData() {
    return this._http.get('http://localhost:3000/getData');
  }
}

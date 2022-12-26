import { TestBed } from '@angular/core/testing';

import { HivedataService } from './hivedata.service';

describe('HivedataService', () => {
  let service: HivedataService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(HivedataService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});

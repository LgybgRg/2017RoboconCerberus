/*
  if (DS4.Getr()<20) {
    counter_L = 1;
  }
  else
    counter_L = 0;

  Counter_L.newData = counter_L;

  if (Counter_L.oldData == 0 && Counter_L.newData == 1)
    flag_L = 1;
  else
    flag_L = 0;

  Counter_L.oldData = Counter_L.newData;

  if (flag_L) {
    old_enc_x = enc_x.GetCount();
    old_enc_y = enc_y.GetCount();
  }

  if (counter_L) {
   int deg_mod=GetAngle_deg(old_enc_x,old_enc,y,enc_x.GetCount(),enc_y.GetCount());
   long distance_mod=GetDistance(old_enc_x,old_enc,y,enc_x.GetCount(),enc_y.GetCount());

   ModExtPower.Update(distance_mod,0);

   Omni.SetSingleData(0,ModExtPower.GetPower());
   Omni.SetSingleData(1, deg_mod >> 8 & 0x00ff);
    Omni.SetSingleData(2, deg_mod & 0x00ff);
  }
*/

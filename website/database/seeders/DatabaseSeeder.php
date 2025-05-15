<?php

namespace Database\Seeders;

use App\Models\User;
// use Illuminate\Database\Console\Seeds\WithoutModelEvents;
use Illuminate\Database\Seeder;

class DatabaseSeeder extends Seeder
{
    /**
     * Seed the application's database.
     */
    public function run(): void
    {
        $this->call(ShieldSeeder::class);
        $this->call(AdminSeeder::class);
//        $this->call(GuruSeeder::class);
        $this->call(JadwalSeeder::class);
        $this->call(DeviceSeeder::class);
    }
}

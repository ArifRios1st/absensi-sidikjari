<?php

namespace App\Filament\App\Resources\GuruResource\Pages;

use App\Filament\App\Resources\GuruResource;
use App\Models\Device;
use App\Models\Fingerprint;
use App\Models\Guru;
use Filament\Actions;
use Filament\Resources\Pages\CreateRecord;
use Filament\Notifications\Notification;

class CreateGuru extends CreateRecord
{
    protected static string $resource = GuruResource::class;

    protected function afterValidate(): void
    {
        if ((Fingerprint::count() + 3) > Device::kapasitasDevice()) {
            Notification::make()
                ->danger()
                ->title('Gagal !')
                ->body('Kapasitas sensor Fingerprint tidak cukup !')
                ->send();

            $this->halt();
        }
    }

    protected function afterCreate(): void
    {
        $guru = $this->getRecord();

        $usedIds = Fingerprint::select('id')->get()->pluck('id')->toArray();
        $availableIds = array_diff(range(1, Device::kapasitasDevice()), $usedIds);
        $fingerprintIds = array_slice($availableIds, 0, 3);

        foreach ($fingerprintIds as $id) {
            Fingerprint::create([
                'id' => $id,
                'id_guru' => $guru->id,
            ]);
        }
    }
}

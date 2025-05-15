<?php

namespace App\Filament\App\Pages;

use Filament\Pages\Page;
use App\Models\Guru;
use Carbon\Carbon;
use Illuminate\Http\Request;
use Illuminate\Support\Collection;
use Livewire\Attributes\Url;

class AbsensiPage extends Page
{
    protected static ?string $navigationGroup = 'Data Absensi';
    protected static ?string $navigationIcon = 'heroicon-o-calendar';

    protected static string $view = 'filament.app.pages.absensi';
    protected static ?string $slug = 'absensi';
    protected ?string $maxContentWidth = 'full';

    #[Url]
    public ?string $selectedDate = null;

    public array $absensiGuru = [];

    public function mount(Request $request)
    {
        if($this->selectedDate == null) {
            $this->selectedDate = now();
        }
    }

    public function getViewData(): array
    {
        return [
            'dates' => $this->getDates(),
            'dataGuru' => $this->getDataGuru(),
            'absensiGuru' => $this->getAbsensiGuru(),
        ];
    }

    public function getDates(): array
    {
        $currentDate = Carbon::parse($this->selectedDate);
        return [
            'previous' => $currentDate->copy()->subMonthNoOverflow()->translatedFormat('F Y'),
            'current' => $currentDate->translatedFormat('F Y'),
            'next' => $currentDate->copy()->addMonthNoOverflow()->translatedFormat('F Y'),
        ];
    }


    public function previousMonth(): void
    {
        $this->selectedDate = Carbon::parse($this->selectedDate)->subMonthNoOverflow();
    }

    public function nextMonth(): void
    {
        $this->selectedDate = Carbon::parse($this->selectedDate)->addMonthNoOverflow();
    }

    public function getDataGuru(): Collection
    {
        return Guru::pluck('nama', 'id');
    }

    public function getAbsensiGuru(): array
    {
        $start = Carbon::parse($this->selectedDate)->copy()->startOfMonth();
        $end = Carbon::parse($this->selectedDate)->copy()->endOfMonth();

        $dataGuru = Guru::query()
        ->with(['dataAbsensi' => function($query) use ($start, $end) {
            $query->whereBetween('tanggal', [$start, $end]);
        }])
        ->get();

        $details = [];
        foreach($dataGuru as $guru) {
            $startDate = $start->copy();
            while($startDate->lte($end)) {
                $data = $guru->dataAbsensi->where('tanggal', $startDate)->first();
                $details[$guru->id][$startDate->format('d')]['jam_masuk'] = $data->jam_masuk ?? null;
                $details[$guru->id][$startDate->format('d')]['jam_keluar'] = $data->jam_keluar ?? null;

                $startDate->addDay();
            }
        }

        $this->absensiGuru = $details;

        return $details;
    }
}

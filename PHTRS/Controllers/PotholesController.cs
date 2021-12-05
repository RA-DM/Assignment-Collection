using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Mvc.Rendering;
using Microsoft.EntityFrameworkCore;
using PHTRS.Data;
using PHTRS.Models;

namespace PHTRS.Controllers
{
    public class PotholesController : Controller
    {
        private readonly PHTRSContext _context;

        public PotholesController(PHTRSContext context)
        {
            _context = context;
        }


        // GET: Potholes
        public async Task<IActionResult> Index(string searchString)
        {
            var potholes = from p in _context.Pothole select p;
            if (!String.IsNullOrEmpty(searchString))
            {
                potholes = potholes.Where(p => p.Street.Contains(searchString));
            }
            return View(await potholes.ToListAsync());
        }

        // GET: Potholes/Details/5
        public async Task<IActionResult> Details(int? id)
        {
            if (id == null)
            {
                return NotFound();
            }

            var pothole = await _context.Pothole
                .FirstOrDefaultAsync(m => m.Id == id);
            if (pothole == null)
            {
                return NotFound();
            }

            return View(pothole);
        }

        // GET: Potholes/Create
        public IActionResult Create()
        {
            return View();
        }

        // POST: Potholes/Create
        // To protect from overposting attacks, enable the specific properties you want to bind to.
        // For more details, see http://go.microsoft.com/fwlink/?LinkId=317598.
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> Create([Bind("Id,Size,Street,Location,District,Priority,ReportDate")] Pothole pothole)
        {
            if (ModelState.IsValid)
            {
                pothole.editPriority();
                _context.Add(pothole);
                await _context.SaveChangesAsync();
                return RedirectToAction(nameof(Index));
            }
            return View(pothole);
        }

        // GET: Potholes/Edit/5
        public async Task<IActionResult> Edit(int? id)
        {
            if (id == null)
            {
                return NotFound();
            }

            var pothole = await _context.Pothole.FindAsync(id);
            if (pothole == null)
            {
                return NotFound();
            }
            return View(pothole);
        }

        // POST: Potholes/Edit/5
        // To protect from overposting attacks, enable the specific properties you want to bind to.
        // For more details, see http://go.microsoft.com/fwlink/?LinkId=317598.
        [HttpPost]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> Edit(int id, [Bind("Id,Size,Street,Location,District,Priority,ReportDate")] Pothole pothole)
        {
            if (id != pothole.Id)
            {
                return NotFound();
            }

            if (ModelState.IsValid)
            {
                try
                {
                    pothole.editPriority();
                    _context.Update(pothole);
                    await _context.SaveChangesAsync();
                }
                catch (DbUpdateConcurrencyException)
                {
                    if (!PotholeExists(pothole.Id))
                    {
                        return NotFound();
                    }
                    else
                    {
                        throw;
                    }
                }
                return RedirectToAction(nameof(Index));
            }
            return View(pothole);
        }

        // GET: Potholes/Delete/5
        public async Task<IActionResult> Delete(int? id)
        {
            if (id == null)
            {
                return NotFound();
            }

            var pothole = await _context.Pothole
                .FirstOrDefaultAsync(m => m.Id == id);
            if (pothole == null)
            {
                return NotFound();
            }

            return View(pothole);
        }

        // POST: Potholes/Delete/5
        [HttpPost, ActionName("Delete")]
        [ValidateAntiForgeryToken]
        public async Task<IActionResult> DeleteConfirmed(int id)
        {
            var pothole = await _context.Pothole.FindAsync(id);
            _context.Pothole.Remove(pothole);
            await _context.SaveChangesAsync();
            return RedirectToAction(nameof(Index));
        }

        private bool PotholeExists(int id)
        {
            return _context.Pothole.Any(e => e.Id == id);
        }
    }
}
